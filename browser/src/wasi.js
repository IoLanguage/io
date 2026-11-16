// Minimal WASI shim, one per VM.
// The reactor binary still imports WASI symbols; we only support what the VM
// actually calls. stdout/stderr from fd_write is buffered on the VM (vm.stdout)
// from construction, since it can arrive before io_init installs printCallback.

export function createWasi(vm) {
	const view = () => new DataView(vm.memory.buffer);

	return {
		// fd_write(fd, iovs_ptr, iovs_len, nwritten_ptr) -> errno
		fd_write(fd, iovs_ptr, iovs_len, nwritten_ptr) {
			const v = view();
			const bytes = new Uint8Array(vm.memory.buffer);
			let totalWritten = 0;

			for (let i = 0; i < iovs_len; i++) {
				const base = v.getUint32(iovs_ptr + i * 8, true);
				const len = v.getUint32(iovs_ptr + i * 8 + 4, true);
				if (fd === 1 || fd === 2) {
					vm.stdout += new TextDecoder().decode(bytes.subarray(base, base + len));
				}
				totalWritten += len;
			}

			v.setUint32(nwritten_ptr, totalWritten, true);
			return 0; // ESUCCESS
		},

		// fd_read(fd, iovs_ptr, iovs_len, nread_ptr) -> errno
		fd_read(fd, iovs_ptr, iovs_len, nread_ptr) {
			view().setUint32(nread_ptr, 0, true);
			return 0; // EOF
		},

		// fd_close(fd) -> errno
		fd_close(fd) { return 0; },

		// fd_seek(fd, offset_lo, offset_hi, whence, newoffset_ptr) -> errno
		fd_seek(fd, offset_lo, offset_hi, whence, newoffset_ptr) {
			view().setBigUint64(newoffset_ptr, 0n, true);
			return 0;
		},

		// fd_fdstat_get(fd, buf) -> errno
		fd_fdstat_get(fd, buf) {
			const v = view();
			v.setUint8(buf, fd <= 2 ? 2 : 4); // CHARACTER_DEVICE for stdio
			v.setUint16(buf + 2, 0, true);     // flags
			v.setBigUint64(buf + 8, 0n, true); // rights_base
			v.setBigUint64(buf + 16, 0n, true); // rights_inheriting
			return 0;
		},

		// fd_fdstat_set_flags(fd, flags) -> errno
		fd_fdstat_set_flags(fd, flags) { return 0; },

		// fd_filestat_set_size(fd, size) -> errno
		fd_filestat_set_size(fd, size) { return 70; }, // ENOSYS

		// fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) -> errno
		fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) {
			view().setUint32(bufused_ptr, 0, true);
			return 0;
		},

		// fd_renumber(fd, to) -> errno
		fd_renumber(fd, to) { return 70; }, // ENOSYS

		// fd_prestat_get(fd, buf) -> errno
		fd_prestat_get(fd, buf) { return 8; }, // EBADF — no preopened dirs

		// fd_prestat_dir_name(fd, path, path_len) -> errno
		fd_prestat_dir_name(fd, path, path_len) { return 8; }, // EBADF

		// path_open — no filesystem
		path_open() { return 76; }, // ENOTCAPABLE

		// path_create_directory(fd, path, path_len) -> errno
		path_create_directory(fd, path, path_len) { return 76; },

		// path_filestat_get(fd, flags, path, path_len, buf) -> errno
		path_filestat_get(fd, flags, path, path_len, buf) { return 76; },

		// path_remove_directory(fd, path, path_len) -> errno
		path_remove_directory(fd, path, path_len) { return 76; },

		// path_rename(old_fd, old_path, old_path_len, new_fd, new_path, new_path_len) -> errno
		path_rename(old_fd, old_path, old_path_len, new_fd, new_path, new_path_len) { return 76; },

		// path_unlink_file(fd, path, path_len) -> errno
		path_unlink_file(fd, path, path_len) { return 76; },

		// clock_time_get(clock_id, precision, time_ptr) -> errno
		clock_time_get(clock_id, precision, time_ptr) {
			const now = BigInt(Math.floor(performance.now() * 1_000_000)); // ns
			view().setBigUint64(time_ptr, now, true);
			return 0;
		},

		// environ_sizes_get(count_ptr, buf_size_ptr) -> errno
		environ_sizes_get(count_ptr, buf_size_ptr) {
			const v = view();
			v.setUint32(count_ptr, 0, true);
			v.setUint32(buf_size_ptr, 0, true);
			return 0;
		},

		// environ_get(environ_ptr, buf_ptr) -> errno
		environ_get(environ_ptr, buf_ptr) { return 0; },

		// args_sizes_get(argc_ptr, buf_size_ptr) -> errno
		args_sizes_get(argc_ptr, buf_size_ptr) {
			const v = view();
			v.setUint32(argc_ptr, 0, true);
			v.setUint32(buf_size_ptr, 0, true);
			return 0;
		},

		// args_get(argv_ptr, buf_ptr) -> errno
		args_get(argv_ptr, buf_ptr) { return 0; },

		// proc_exit(code)
		proc_exit(code) {
			throw new Error(`Io called exit(${code})`);
		},

		// random_get(buf, buf_len) -> errno
		random_get(buf, buf_len) {
			crypto.getRandomValues(new Uint8Array(vm.memory.buffer, buf, buf_len));
			return 0;
		},

		// sched_yield() -> errno
		sched_yield() { return 0; },

		// poll_oneoff — stub
		poll_oneoff(in_ptr, out_ptr, nsubscriptions, nevents_ptr) {
			view().setUint32(nevents_ptr, 0, true);
			return 0;
		},
	};
}
