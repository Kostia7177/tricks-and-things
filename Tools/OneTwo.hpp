#pragma once

// a pair of types with guarantee-different sizes.
// used within any sfinae-based detectors.
typedef char One;
struct Two { One two[2]; };
