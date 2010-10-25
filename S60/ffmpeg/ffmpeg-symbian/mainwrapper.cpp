#include <e32base.h>
#include <e32cons.h>
#include <stdio.h>

//extern "C" int main(int argc, char *argv[]);
extern "C" int entrypoint();

CConsoleBase* console;

extern "C" void doWait(int i) {
	printf("wait %d\n", i);
//	console->Getch();
	getchar();
}

void callCodeL() {
//	console = Console::NewL(_L("ffmpeg"), TSize(KConsFullScreen, KConsFullScreen));
//	CleanupStack::PushL(console);
//	console->Printf(_L("Starting..."));
//	console->Getch();
	printf("starting...\n");
	getchar();
	entrypoint();
	getchar();
//	console->Getch();
//	CleanupStack::PopAndDestroy(console);
}

GLDEF_C TInt E32Main() {
//	return main(0, NULL);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(err, callCodeL());
	if (err)
		User::Panic(_L("avtest.exe"), err);
	delete cleanup;
	return 0;
}

