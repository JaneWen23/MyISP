
# include "Filtering.hpp"

// FIR or IIR; conv or FFT; 1D and 2D, 2D separate to 1D

// H(z) = (b0 + b1*z^-1 + b2*z^-2 + ...) / (a0 + a1*z^-2 + a2*z^-2 + ...) = Y(z) / X(z)
// y[n] = (b0*x[n] + b1*x[n-1] +b2*x[n-2] + ... - a1*y[n-1] - a2*y[n-2]) / a0