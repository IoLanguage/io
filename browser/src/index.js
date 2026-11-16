// Io VM for JavaScript.
//
//   import { Io } from "./src/index.js";
//   const io = await Io.load();
//   const { status, output } = await io.eval('list(1, 2, 3) map(x, x * x) sum');

export { Io } from "./vm.js";
