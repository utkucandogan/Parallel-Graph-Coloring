#pragma once

// These exist so that references and arrays do not mix up in function definitions
#define ref_type(X) X*
#define ref(X) (*X)
