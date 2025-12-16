#include "wolfram.hpp"


// elementary functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcADD(calc_context *calc_params) { return calc_params->left_val + calc_params->right_val; }
node_t *diffADD(diff_context *diff_params) { return ADD_(dL, dR); }

double calcSUB(calc_context *calc_params) { return calc_params->left_val - calc_params->right_val; }
node_t *diffSUB(diff_context *diff_params) { return SUB_(dL, dR); }

double calcMUL(calc_context *calc_params) { return calc_params->left_val * calc_params->right_val; }
node_t *diffMUL(diff_context *diff_params) { return ADD_(MUL_(dL, R), MUL_(L, dR)); }

double calcDIV(calc_context *calc_params) { return calc_params->left_val / calc_params->right_val; } // div by zero!!!
node_t *diffDIV(diff_context *diff_params) { return DIV_(SUB_(MUL_(dL, R), MUL_(dR, L)), POW_(R, NUM_(2.0))); }

double calcPOW(calc_context *calc_params) { return pow(calc_params->left_val, calc_params->right_val); } // zero to the power of zero!!!
node_t *diffPOW(diff_context *diff_params) 
{ 
    node_t *base = diff_params->node_left;
    node_t *exponent = diff_params->node_right;
    
    if (base->type == ARG_NUM)
        return MUL_(MUL_(LN_(L), POW_(L, R)), dR);

    else if (exponent->type == ARG_NUM)
        return MUL_(MUL_(R, POW_(L, SUB_(R, NUM_(1.0)))), dL);

    else 
        return MUL_(POW_(L, R), ADD_(MUL_(dR, LN_(L)), MUL_(dL, DIV_(R, L))));
}

double calcSQRT(calc_context *calc_params) { return sqrt(calc_params->right_val); } // div by zero!!!
node_t *diffSQRT(diff_context *diff_params) { return DIV_(dR, MUL_(NUM_(2), SQRT_(R))); }

//---------------------------------------------------------------------------------------------------------------------------------------------------------



// exponential functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcE(calc_context *calc_params) { return exp(calc_params->right_val); }
node_t *diffE(diff_context *diff_params) { return MUL_(E_(R), dR); }

double calcLN(calc_context *calc_params) { return log(calc_params->right_val); }
node_t *diffLN(diff_context *diff_params) { return DIV_(dR, R); }

double calcLOG(calc_context *calc_params) { return log(calc_params->right_val) / log(calc_params->left_val); }
node_t *diffLOG(diff_context *diff_params) { return DIV_(dR, MUL_(L, LN_(R))); }
//---------------------------------------------------------------------------------------------------------------------------------------------------------



// trigonometric functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcSIN(calc_context *calc_params) { return sin(calc_params->right_val); }
node_t *diffSIN(diff_context *diff_params) { return MUL_(COS_(R), dR); }

double calcCOS(calc_context *calc_params) { return cos(calc_params->right_val); }
node_t *diffCOS(diff_context *diff_params) { return MUL_(NUM_(-1.0), MUL_(SIN_(R), dR)); }

double calcTAN(calc_context *calc_params) { return tan(calc_params->right_val); }
node_t *diffTAN(diff_context *diff_params) { return DIV_(dR, POW_(COS_(R), NUM_(2.0))); }

double calcCOT(calc_context *calc_params) { return 1 / tan(calc_params->right_val); }
node_t *diffCOT(diff_context *diff_params) { return MUL_(DIV_(NUM_(-1.0), POW_(SIN_(R), NUM_(2.0))), dR); }

//---------------------------------------------------------------------------------------------------------------------------------------------------------



// hyperbolic functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcSINH(calc_context *calc_params) { return (exp(calc_params->right_val) - exp(-calc_params->right_val)) / 2; }
node_t *diffSINH(diff_context *diff_params) { return MUL_(COSH_(R), dR); }

double calcCOSH(calc_context *calc_params) { return (exp(calc_params->right_val) + exp(-calc_params->right_val)) / 2; }
node_t *diffCOSH(diff_context *diff_params) { return MUL_(SINH_(R), dR); }

double calcTANH(calc_context *calc_params) { return calcSINH(calc_params) / calcCOSH(calc_params); }
node_t *diffTANH(diff_context *diff_params) { return DIV_(dR, POW_(COSH_(R), NUM_(2.0))); }

double calcCOTH(calc_context *calc_params) { return calcCOSH(calc_params) / calcSINH(calc_params); }
node_t *diffCOTH(diff_context *diff_params) { return MUL_(DIV_(NUM_(-1.0), POW_(SINH_(R), NUM_(2.0))), dR); }

//---------------------------------------------------------------------------------------------------------------------------------------------------------



// inverse trigonometric functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcARCSIN(calc_context *calc_params) { return asin(calc_params->right_val); }
node_t *diffARCSIN(diff_context *diff_params) { return DIV_(dR, POW_(SUB_(NUM_(1.0), POW_(R, NUM_(2.0))), NUM_(0.5))); }

double calcARCCOS(calc_context *calc_params) { return acos(calc_params->right_val); }
node_t *diffARCCOS(diff_context *diff_params) { return MUL_(DIV_(NUM_(-1.0), POW_(SUB_(NUM_(1.0), POW_(R, NUM_(2.0))), NUM_(0.5))), dR); }

double calcARCTAN(calc_context *calc_params) { return atan(calc_params->right_val); }
node_t *diffARCTAN(diff_context *diff_params) { return DIV_(dR, ADD_(NUM_(1.0), POW_(R, NUM_(2.0)))); }

double calcARCCOT(calc_context *calc_params) { return acot(calc_params->right_val); }
node_t *diffARCCOT(diff_context *diff_params) { return MUL_(DIV_(NUM_(-1.0), ADD_(NUM_(1.0), POW_(R, NUM_(2.0)))), dR); }

//---------------------------------------------------------------------------------------------------------------------------------------------------------


// inverse hyperbolic functions
//---------------------------------------------------------------------------------------------------------------------------------------------------------

double calcARCSINH(calc_context *calc_params) { return asinh(calc_params->right_val); }
node_t *diffARCSINH(diff_context *diff_params) { return DIV_(dR, POW_(ADD_(NUM_(1.0), POW_(R, NUM_(2.0))), NUM_(0.5))); }

double calcARCCOSH(calc_context *calc_params) { return acosh(calc_params->right_val); }
node_t *diffARCCOSH(diff_context *diff_params) { return MUL_(DIV_(NUM_(-1.0), POW_(SUB_(POW_(R, NUM_(2.0)), NUM_(1.0)), NUM_(0.5))), dR); }

double calcARCTANH(calc_context *calc_params) { return atanh(calc_params->right_val); }
node_t *diffARCTANH(diff_context *diff_params) { return DIV_(dR, SUB_(NUM_(1.0), POW_(R, NUM_(2.0)))); }

double calcARCCOTH(calc_context *calc_params) { return acoth(calc_params->right_val); }
node_t *diffARCCOTH(diff_context *diff_params) { return DIV_(dR, SUB_(NUM_(1.0), POW_(R, NUM_(2.0)))); }

//---------------------------------------------------------------------------------------------------------------------------------------------------------