#include "../../../bmc/core/definitions.h"
#include "../../../bmc/core/compatibility.h"
#include "../../../bmc/core/util.h"

int main(){
	double y[8] = {1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0 }; 
	int y_size = 8;
	double_check_oscillations(y,y_size);
}