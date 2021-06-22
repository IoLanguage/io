true setSlot("and", Object getSlot("evalArg"))
false setSlot("and", false)

nil setSlot("ifNil", Object getSlot("evalArgAndReturnSelf"))
Object setSlot("ifNil", Object getSlot("thisContext"))

nil setSlot("ifNilEval", Object getSlot("evalArg"))
Object setSlot("ifNilEval", Object getSlot("thisContext"))

Message do (
	setSlot("isRightOperator",
		method(
			arguments size ==(0) and(OperatorTable rightOperators contains(name))
		)
	)

	setSlot("isLeftOperator",
		method(
			OperatorTable leftOperators contains(name)
		)
	)

	setSlot("operatorPrecedence",
		method(
			if(arguments size ==(0), OperatorTable operatorPrecedence(name), nil)
		)
	)

	setSlot("opShuffleIo",
		method(
			setSlot("levels", Levels clone)
			setSlot("expressions", List clone with(self))

			while(expressions size >=(1),
				setSlot("n", expressions pop)
				while(n,
					setSlot("a", n)
					while(a,
						levels attach(a, expressions)
						expressions appendSeq(a arguments)
						updateSlot("a", a attached)
					)
					levels nextMessage
					updateSlot("n", n next)
				)
			)
			self
		)
	)

		setSlot("opShuffle", getSlot("opShuffleC"))

		useCShuffle := method(self setSlot("opShuffle", getSlot("opShuffleC")))
		useIoShuffle := method(self setSlot("opShuffle", getSlot("opShuffleIo")))

		setSlot("OperatorTable",
				Map clone do(
						setSlot("maxPrecedence", nil)

						setSlot("contains", getSlot("hasKey"))

						setSlot("operatorPrecedence",
								method(name,
										rightOperators at(name) ifNilEval(leftOperators at(name))
								)
						)

						setSlot("leftOperators",
								thisContext clone do(
										atPut(":=", 0)
										atPut("=", 0)
										updateSlot("maxPrecedence", 0)
								)
						)

						setSlot("rightOperators",
								thisContext clone do(
										atPut("@",   0)
										atPut("@@",  0)
										atPut("'",   0)
										atPut(".",   0)
										atPut("?",   0)
										atPut("(",   0)
										atPut(")",   0)
										atPut(":",   0)

										atPut("**",  1)

										atPut("++",  2)
										atPut("--",  2)

										atPut("*",   3)
										atPut("/",   3)
										atPut("%",   3)

										atPut("+",   4)
										atPut("-",   4)

										atPut("<<",  5)
										atPut(">>",  5)

										atPut(">",   6)
										atPut("<",   6)
										atPut("<=",  6)
										atPut(">=",  6)

										atPut("==",  7)
										atPut("!=",  7)

										atPut("&",   8)

										atPut("^",   9)

										atPut("|",   10)

										atPut("and", 11)
										atPut("&&",  11)

										atPut("or",  12)
										atPut("||",  12)

										atPut("..",  13)

										atPut("=",   14)
										atPut("+=",  14)
										atPut("-=",  14)
										atPut("*=",  14)
										atPut("/=",  14)
										atPut("%=",  14)
										atPut("&=",  14)
										atPut("^=",  14)
										atPut("|=",  14)
										atPut("<<=", 14)
										atPut(">>=", 14)
										atPut(":=",  14)
										atPut("<-",  14)
										atPut("<->", 14)
										atPut("->",  14)

										atPut("return", 15)

										atPut(",", 16)

										updateSlot("maxPrecedence", 16)
								)
						)
				)
		)

		setSlot("Levels",
				Object clone do(
						# TODO: Speed up by reusing Level objects (removeAllSlots)
						# Have a pool of level objects to reuse
						setSlot("stack", nil)

						setSlot("init",
								method(
										reset
								)
						)

						setSlot("reset",
								method(
										updateSlot("stack", List clone with(Level clone setRootLevel))
								)
						)

						setSlot("nextMessage",
								method(
										stack reverseForeach(msg, msg finish)
										reset
								)
						)

						setSlot("currentLevel",
								method(
										stack last
								)
						)

						setSlot("currentPrecedence",
								method(
										currentLevel precedence
								)
						)

						setSlot("popDownTo",
								method(targetLevel,
										while(currentPrecedence <=(targetLevel) and(currentLevel isNotAttachingArgs),
												currentLevel finish
												stack pop
										)
								)
						)

						setSlot("attachToTopAndPush",
								method(msg,
										currentLevel attachAndReplace(msg)
										stack push(Level clone setAwaitingFirstArg(msg))
								)
						)

						setSlot("attach",
								method(msg, expressions,
										if(msg isLeftOperator,
												attachLeftOperator(msg, expressions)
												return
										)
										if(msg isRightOperator,
												attachRightOperator(msg)
												return
										)

										currentLevel attachAndReplace(msg)
								)
						)

						setSlot("attachRightOperator",
								method(msg,
										if(msg argCount >(0),
												currentLevel setAlreadyHasArgs(msg)
										,
												popDownTo(msg operatorPrecedence)
												attachToTopAndPush(msg)
										)
								)
						)

						setSlot("attachLeftOperator",
								method(msg, expressions,
										setSlot("attaching", currentLevel msg)

										attaching ifNil(
												Exception raise("compile error: " ..(msg name) ..(" requires a symbol to its left."))
										)

										if(attaching argCount >(0),
												Exception raise("compile error: The symbol to the left of " ..(msg name) ..(" cannot have arguments."))
										)

										setSlot("slotName", attaching name)
										setSlot("quotedSlotName", slotName Sequence clone appendSeq("\"", slotName asMutable replaceSeq("\"", "\\\""), "\""))
										setSlot("slotNameMessage", Message clone setName(quotedSlotName) setCachedResult(slotName))

										attaching appendArg(slotNameMessage)

										setSlot("setSlotName",
												if(msg name ==(":="),
														if(slotName at(0) isUppercase, "setSlotWithType", "setSlot")
												,
														"updateSlot"
												)
										)

										attaching setName(setSlotName)
										currentLevel setAssignment

										if(msg argCount >(0),
												attaching appendArg(
														Message clone setName("") appendArg(msg argAt(0)) setAttached(msg attached)
												)
										,
												msg attached ifNil(
														Exception raise("compile error: " ..(msg name) ..(" must be followed by a value."))
												)

												attaching appendArg(msg attached)
										)

										expressions append(msg attached)

										attaching setAttached(nil) removeCachedResult
										msg setAttached(nil)
								)
						)

						setSlot("Level",
								Object clone do(
										setSlot("maxPrecedence", 18)

										setSlot("msg", nil)
										setSlot("precedence", nil)
										setSlot("attach", nil)
										setSlot("attachBehaviourName", nil)

										setSlot("finish",
												method(
														if(msg, msg setAttached(nil))
														changeAttachBehaviour("attachNop")
												)
										)

										setSlot("changeAttachBehaviour",
												method(name,
														updateSlot("attachBehaviourName", name)
														updateSlot("attach", self getSlot(name))
												)
										)

										setSlot("attachAttach", method(m, msg setAttached(m)))
										setSlot("attachArg", method(m, msg appendArg(m)))
										setSlot("attachNew", method(m, updateSlot("msg", m)))
										setSlot("attachNop", nil)

										changeAttachBehaviour("attachNop")

										setSlot("attachAndReplace",
												method(m,
														attach(m)
														updateSlot("msg", m)
														changeAttachBehaviour("attachAttach")
												)
										)

										setSlot("setAwaitingFirstArg",
												method(m,
														updateSlot("msg", m)
														self updateSlot("precedence", m operatorPrecedence)
														changeAttachBehaviour("attachArg")
														self
												)
										)

										setSlot("setAlreadyHasArgs",
												method(m,
														changeAttachBehaviour("attachAttach")
														updateSlot("msg", m)
														self
												)
										)

										setSlot("setRootLevel",
												method(
														updateSlot("precedence", maxPrecedence)
														changeAttachBehaviour("attachNew")
														self
												)
										)

										setSlot("setAssignment", method(changeAttachBehaviour("attachAttach")))

										setSlot("isNotAttachingArgs",
												method(self getSlot("attachArg") !=(self getSlot("attach")))
										)
								)
						)
				)
		)
)

