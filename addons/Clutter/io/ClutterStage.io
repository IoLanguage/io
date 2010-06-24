ClutterStage do(
  forward := method(
    self actor hasSlot(call message name) ifTrue(
      self actor doMessage(call message)))

  # Caches the result
  default = default()
)
