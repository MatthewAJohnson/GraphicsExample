#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/Window.h"
#include "Renderer.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG
// {,,msvcr110d.dll}*__p__crtBreakAlloc()
/* Memory leak testing*/
int main()
{
	Window w("My First OpenGL 3 Triangle!", 1240, 900, false);
	//Window w("My First Graphics Coursework!", 1920, 1080, true);
	if(!w.HasInitialised())
	{
		return -1;
	}
	Renderer renderer(w);
	if(!renderer.HasInitialised())
	{
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE))
	{
		if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_R)) 
		{
		 
		renderer.ResetScene();
		}
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}
	_CrtDumpMemoryLeaks(); // print any memory leaks
    //system("pause");
	return 0;
}