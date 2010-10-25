#include "Tmndec.h"

int InitDecoder(){
	return InitH263Decoder();
}
void ReleaseDecoder(){
	ExitH263Decoder();
}

