using System;

namespace Oxel
{
    public enum PlaneSideResult
    {
        Intersects,	// On plane
        Inside,		// Negative side of plane / 'inside' half-space
        Outside		// Positive side of plane / 'outside' half-space
    }
}
