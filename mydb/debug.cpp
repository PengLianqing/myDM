#include "debug.h"

# define DEBUG_TEST 0
# if DEBUG_TEST

int main(){
	LOGE("hello:linux!\n");
	LOGW("hello:linux!\n");
	LOGI("hello:linux!\n");
	LOGD("hello:linux!\n");
	return 0;
}

#endif