# State

High-performance iterative evaluator with aggressive optimizations:
- Frame pooling (no malloc/free in hot path)
- Computed gotos (faster than switch)
- Inline fast paths for common cases
- Local variable caching

