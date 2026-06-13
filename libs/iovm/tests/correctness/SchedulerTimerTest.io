SchedulerTimerTest := UnitTest clone do(
	// Object wait parks the current coroutine on Scheduler's timer queue
	// instead of busy-spinning, so waiting coroutines run concurrently and
	// the VM idles in a single host sleep when nothing is runnable.
	//
	// coroDo (not coroDoLater) is used because it runs the body in the
	// sender's context, so the coroutines can see the test method's locals.

	testWaitReturnsAfterDeadline := method(
		t := Date clone now asNumber
		wait(0.05)
		elapsed := Date clone now asNumber - t
		assertTrue(elapsed >= 0.04)
		assertTrue(Scheduler timers isEmpty)
	)

	testWaitsRunConcurrently := method(
		s := Sequence clone
		t := Date clone now asNumber
		coroDo(wait(0.12); s appendSeq("slow."))
		coroDo(wait(0.04); s appendSeq("fast."))
		wait(0.2)
		elapsed := Date clone now asNumber - t
		assertEquals("fast.slow.", s asString)
		// concurrent waits take ~max(0.12, 0.04, 0.2), not the 0.36 sum
		assertTrue(elapsed < 0.33)
		assertTrue(Scheduler timers isEmpty)
	)

	testTimerWakesDuringYields := method(
		s := Sequence clone
		coroDo(wait(0.05); s appendSeq("timer."))
		deadline := Date clone now asNumber + 1
		while(s size == 0 and(Date clone now asNumber < deadline),
			yield
		)
		s appendSeq("main.")
		assertEquals("timer.main.", s asString)
	)

	testFinishAfterStarterDied := method(
		// Regression test for the dead-ancestor walk in the eval loop:
		// c1's pause starts c2, so c2's parentCoroutine is c1. c1 finishes
		// first, so when c2 finishes its parent chain has a dead link and
		// the eval loop must walk past it to resume this coroutine.
		s := Sequence clone
		c1 := coroFor(wait(0.03); s appendSeq("c1."))
		c2 := coroFor(wait(0.08); s appendSeq("c2."))
		Scheduler yieldingCoros append(c1)
		Scheduler yieldingCoros append(c2)
		wait(0.15)
		assertEquals("c1.c2.", s asString)
		assertTrue(Scheduler timers isEmpty)
	)

	testActorCanWait := method(
		o := Object clone
		o s := Sequence clone
		o job := method(wait(0.03); s appendSeq("done."); s)
		f := o @job
		// touching the future's proxy blocks this coroutine until the
		// actor's wait completes via the scheduler's idle path
		assertEquals("done.", f asString)
	)
)
