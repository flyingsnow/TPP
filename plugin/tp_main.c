#include <stdio.h>
#include <math.h>

#include <GLES2/gl2.h>
#include "SDL.h"
#include "PDL.h"
#include "SDL_rotozoom.h"

#include "fitz.h"
#include "mupdf.h"
#include "muxps.h"
#include "pdfapp.h"



SDL_Surface *Surface;	//Screen surface 
SDL_Surface *content, *page_copy;
SDL_Rect src, dest;

struct PDF_Pitch_s {
	Uint16 x0, y0;
	Uint16 x1, y1;
	float32 distance0;
	float32 distance1;
	float32 offset;
	Uint16 cx, cy;
	float32 scale;
};

typedef struct PDF_Pitch_s PDF_Pitch_t;

PDF_Pitch_t pitch_action;

int fingerNum;
int mouse_state;
SDL_bool pinch;
pdfapp_t app;
SDL_bool Paused = SDL_FALSE;


void hadle_events(SDL_Event event);

static void draw_pdf()
{
	SDL_Rect dest_rec;
    SDL_Surface *optimizedImage = NULL;
    SDL_FreeSurface(page_copy);

    page_copy = SDL_CreateRGBSurfaceFrom(app.image->samples, app.image->w, app.image->h,
            32, app.image->w * 4, 0x00, 0x00, 0x00, 0x00);
    optimizedImage = SDL_DisplayFormat(page_copy); 
    //		content = SDL_CreateRGBSurfaceFrom(app.image->samples, pagewidth,pageheight,
    //			32,pagewidth * 4, 0x00, 0x00, 0x00, 0x00);
    dest_rec.x = (1024 - app.image->w)/2;
	dest_rec.y = 0;
	SDL_FillRect(Surface,NULL,SDL_MapRGBA(Surface->format,45,45,45,0));
    SDL_BlitSurface(optimizedImage,NULL, Surface, &dest_rec);
    SDL_Flip(Surface);
    //		SDL_Delay(100);
}

PDL_bool flip_page(PDL_JSParameters *params)
{
    char *key;
    if(PDL_GetNumJSParams(params) < 1) {
             PDL_JSException(params, "Params not ok");
        return PDL_TRUE;
    }
    key = PDL_GetJSParamString(params,0); 

    pdfapp_onkey(&app,*key);
    draw_pdf();

    return PDL_TRUE;
}

PDL_bool pan_page(PDL_JSParameters *params)
{
    printf("recive pan page command\n");
    return PDL_TRUE;
}

#if 0
// these methods take no parameters.  They're needed when running as a plugin in a hybrid                                                                                                                                                     
// application to pause the rendering loop because plugins to JS apps don't get application                                                                                                                                                   
// activation messages.                                                                                                                                                                                                                       
PDL_bool pause(PDL_JSParameters *params)                                                                                                                                                                                                      
{                                                                                                                                                                                                                                             
	Paused = SDL_TRUE;
    return PDL_TRUE;
}
 
PDL_bool resume(PDL_JSParameters *params)
{
	Paused = SDL_FALSE;

	// push a bogus event into the queue to wake up main thread
	SDL_Event Event;                                                                                                                                                                                                                                                         
	Event.active.type = SDL_ACTIVEEVENT;                                                                                                                                                                                                      
	Event.active.gain = 1;                                                                                                                                                                                                                    
	Event.active.state = 0; /* no state makes this event meaningless */                                                                                                                                                                       
	SDL_PushEvent(&Event);                                                                                                                                                                                                                    
    return PDL_TRUE;
}  
#endif

int main(int argc, char** argv)
{
	int result;
	int fd;
	fz_context *ctx;
	char buf[10];
	int c; 
	int pagewidth, pageheight;
	
	result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	if(result !=  0) {
		printf("Could not init SDL: %s\n", SDL_GetError());
		exit(1);
	}

	PDL_Init(0);
	PDL_SetOrientation(PDL_ORIENTATION_0);
    
    PDL_RegisterJSHandler("flipPage",flip_page);
    PDL_RegisterJSHandler("panPage",pan_page);	
 //   PDL_RegisterJSHandler("pause", pause);																																																									
 //	PDL_RegisterJSHandler("resume", resume);
	
    PDL_JSRegistrationComplete();
    PDL_CallJS("ready", NULL, 0);

	memset(&pitch_action, 0, sizeof(PDF_Pitch_t));
	pitch_action.scale = 1.0;
	
	Surface = SDL_SetVideoMode(0, 0, 0, SDL_SWSURFACE);

	SDL_FillRect(Surface, NULL, 0xF0F0F);
	SDL_Flip(Surface);

	ctx = fz_new_context(NULL, FZ_STORE_DEFAULT);

	pdfapp_init(ctx, &app);

	app.scrw = 1024;
	app.scrh = 768;
	app.pageno = 10;
	
	src.x = 20;
	src.y = 20;
	src.w = 1024;
	src.h = 768;

	dest.x = 0;
	dest.y = 0;
	
	fd = open(argv[1], O_RDONLY, 0666);
	if (fd < 0)	{
		printf("cannot open file\n");
		fz_throw(app.ctx, "cannot open file '%s'", argv[1]);
	}
	pdfapp_open(&app,argv[1],fd,0);
	
	//app.rotate = 90 * 3;
	pdfapp_showpage(&app,1,1,1);
	draw_pdf();
	
#if 0
	content = SDL_CreateRGBSurfaceFrom(app.image->samples, app.image->w, app.image->h,
		32, app.image->w * 4, 0x00, 0x00, 0x00, 0x00);
	

	SDL_BlitSurface(content,NULL, Surface, NULL);
	SDL_Flip(Surface);
#endif

	SDL_Event Event;
	Event.type = SDL_NOEVENT;
	int temp1, temp2;
	
	do {
		if (Paused)	{
			SDL_WaitEvent(&Event);
			if (Event.type == SDL_ACTIVEEVENT)
			{
				if ((Event.active.state & SDL_APPACTIVE) && (Event.active.gain == 1))
				{
					Paused = SDL_FALSE;
				}
			}
		}			
	else {
		while(SDL_PollEvent(&Event)) {
			hadle_events(Event);
//			switch (Event.type) {
#if 0
				case SDL_MOUSEBUTTONDOWN:
						fingerNum = Event.button.which + 1;
                        mouse_state = SDL_MOUSEBUTTONDOWN;
						printf("%2d fingers touch down\n",Event.button.which);
					break;
					
				case SDL_MOUSEBUTTONUP:
						if(fingerNum == 2) { 
							memset(&pitch_action, 0, sizeof(PDF_Pitch_t)-sizeof(float32));
							pinch = SDL_FALSE;
							//pitch_action.scale = 1.0;
						}
						if(fingerNum == 1 && mouse_state == SDL_MOUSEBUTTONDOWN) {
                            if(Event.button.x > (app.image->h / 2)) {
								buf[0] = '.';
                            }
							else {
								buf[0] = ',';
							}
							pdfapp_onkey(&app,buf[0]);
							printf("Flip page\n");
					    	draw_pdf();
						}
					//	printf("%2d fingers touching\n",fingerNum);
                        mouse_state = NULL;	
						printf("%2d fingers touch up\n",Event.button.which);

						if(fingerNum > 0)
							fingerNum -= 1; 
					break;

				case SDL_MOUSEMOTION:
					if(fingerNum == 2) {
							if(Event.motion.which == 0) {
								pitch_action.x0 = Event.motion.x;
								pitch_action.y0 = Event.motion.y;
								}
							if(Event.motion.which == 1) {
								pitch_action.x1 = Event.motion.x;
								pitch_action.y1 = Event.motion.y;
								}
							pitch_action.cx = (pitch_action.x0 + pitch_action.x1)/2;
							pitch_action.cy = (pitch_action.y0 + pitch_action.y1)/2;
							temp1  = pow((pitch_action.x0 - pitch_action.x1),2);
							temp2  = pow((pitch_action.y0 - pitch_action.y1),2);
							pitch_action.distance1 = sqrt(temp1 + temp2);
							
							pitch_action.offset = pitch_action.distance1 - pitch_action.distance0;
							pitch_action.distance0 = pitch_action.distance1;

							pitch_action.offset = (int)pitch_action.offset;
						//	printf("cx:%4d, cy:%4d, offset:%4f \n",pitch_action.cx,
						//		pitch_action.cy, pitch_action.offset);

							#if 1
							if(pitch_action.offset > 0 && pitch_action.offset < 5) {
								#if 0
									app.resolution *= (Uint16)pitch_action.offset;
									if (app.resolution > MAXRES)
										app.resolution = MAXRES;
									pdfapp_showpage(&app, 0, 1, 0);									
									draw_pdf();
								#endif
								pinch = SDL_TRUE;
								pitch_action.scale += 0.008;
								if(pitch_action.scale > 2)
									pitch_action.scale = 2;
								printf("-------------------------------------pinch out\n");
									
							}
							else if(pitch_action.offset < 0) {
								#if 0
									app.resolution /=(Uint16)( abs(pitch_action.offset));
									if (app.resolution < MINRES)
										app.resolution = MINRES;
									pdfapp_showpage(&app, 0, 1, 0);
									draw_pdf();
								#endif
									pinch = SDL_TRUE;
									pitch_action.scale -= 0.008;
									if(pitch_action.scale < 0.9)
										pitch_action.scale = 0.9;

									printf("----------------------------------pinch in\n");
								}
							else {
								pinch = SDL_FALSE;
							}
							#endif
						}
					//printf("which: %d, x = %4d, y = %4d \n",Event.motion.which,
					//	Event.motion.x, Event.motion.y);
                    mouse_state =  SDL_MOUSEMOTION;
					printf("Finger move \n");
                    break;
#endif
/*
				// handle deactivation by pausing our animation
				case SDL_ACTIVEEVENT:
					if ((Event.active.state & SDL_APPACTIVE) &&	(Event.active.gain == 0))
					{
						Paused = SDL_TRUE;
					}
					break;
					
				default:
					break;
//				}
*/
		}

#if 1
		if(pinch) {
			#if 1			
			if(pitch_action.scale == 1.4) {
				app.resolution = 72 * 1.4;
				pdfapp_showpage(&app,0,1,1);
				draw_pdf();
				printf("----increase pdf resolution \n");
			}
			if(pitch_action.scale == 1) {
				app.resolution = 72;
				pdfapp_showpage(&app,0,1,1);
				draw_pdf();
			}
			content = zoomSurface(page_copy,pitch_action.scale,pitch_action.scale,SMOOTHING_OFF);

			SDL_FillRect(Surface, NULL, SDL_MapRGBA(Surface->format, 45, 45, 45, 0));

			SDL_BlitSurface(content,NULL, Surface, NULL);
			SDL_Flip(Surface);
			SDL_FreeSurface(content);
			#else 
			app.resolution *= pitch_action.scale;
			if(app.resolution > MAXRES)
				app.resolution = MAXRES;
			if (app.resolution < MINRES)
				app.resolution = MINRES;
			pdfapp_showpage(&app, 0, 1, 1);
			draw_pdf();
			#endif
		}
#endif
			SDL_Delay(0);
		}
	}while(Event.type != SDL_QUIT);
	//cleanup
	SDL_FreeSurface(content);
	SDL_FreeSurface(Surface);
	
	pdfapp_close(&app);

	PDL_Quit();
	SDL_Quit();
	
	return 0;
}


int16_t distance = 0;
uint16_t angle = 0;
uint8_t  direction = 0;
uint8_t fingers = 0;
SDL_bool first = SDL_FALSE;
uint16_t pos_start_x, pos_start_y;
uint16_t pos_move_x, pos_move_y;

uint32_t touch_start_time = 0;
uint32_t pre_tap_end_time = 0;
uint32_t hold_timer = 0;
uint16_t prev_tap_posX, pre_tapY;
uint16_t offset_x, offset_y;

SDL_bool mousedown = SDL_FALSE;

SDL_Event event_start, event_move, event_end;

enum {
	GESTURE_NULL = SDL_USEREVENT + 1,
	GESTURE_HOLD,
	GESTURE_TAP,	
	GESTURE_SWIPE,
	GESTURE_DRAG,
	GESTURE_TRANSFORM,
}GESTURE_TYPE;
	
//GESTURE_TYPE 
uint8_t _gesture = GESTURE_NULL;

uint8_t count_fingers(SDL_Event event) 
{
	if(SDL_MOUSEBUTTONDOWN == event.type)
		return event.button.which + 1;
	else 
		return 0;
}


void triggle_event(uint8_t gesture_type) 
{
	SDL_Event Event;																																																														 
	Event.user.type = gesture_type; 																																																	 
	SDL_PushEvent(&Event);	
}

SDL_NewTimerCallback hold(Uint32 interval, void *param)
{
	// push a bogus event into the queue to wake up main thread
	if(_gesture == GESTURE_HOLD) {
		SDL_Event Event;                                                                                                                                                                                                                                                         
		Event.user.type = GESTURE_HOLD;                                                                                                                                                                                                      
		SDL_PushEvent(&Event);  
	}
    return SDL_FALSE;
}  


void gesture_hold(SDL_Event event) 
{
	_gesture = GESTURE_HOLD;
	if(NULL  == SDL_AddTimer(800,hold,NULL))
		exit(0);
}

SDL_bool gesture_transfrom(SDL_Event event) 
{
	if(fingers != 2) {
		return SDL_FALSE;
	}
	_gesture = GESTURE_TRANSFORM;
	
	triggle_event(_gesture);
	return SDL_TRUE;


}

void gesture_drag(SDL_Event event) 
{
	int16_t distance_x = pos_move_x - pos_start_x;
	int16_t distance_y = pos_move_y - pos_start_y;

	distance = sqrt(distance_x * distance_x + distance_y * distance_y);

	if(distance > 20 || _gesture == GESTURE_DRAG) {
		_gesture = GESTURE_DRAG;

		if(first) {
			first = SDL_FALSE;
		}
		triggle_event(_gesture);
		printf("%d drag distance  ",distance);
	}

}


void hadle_events(SDL_Event event)
{
	static uint32_t count_hold;
	switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
					first = SDL_TRUE;
					mousedown = SDL_TRUE;
					event_start = event;
					touch_start_time = SDL_GetTicks();
					fingers = count_fingers(event);
					pos_start_x = event.button.x;
					pos_start_y = event.button.y;
					printf("%3d fingers on screen:%4d, %4d\n",fingers, event.button.x, event.button.y);
					gesture_hold(event);
				break;
			case SDL_MOUSEMOTION:
					if(!mousedown)
						break;
					event_move = event;
					pos_move_x = event.button.x;
					pos_move_y = event.button.y;
					if(!gesture_transfrom(event)) {
						gesture_drag(event);
					}
				//	printf("finger move to: %4d, %d\n", event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				mousedown = SDL_FALSE;
				_gesture = GESTURE_NULL;
				break;
			default:
				break;			

		}

	switch(event.user.type) {
		case GESTURE_HOLD:
				printf("Fire Hold .....................%d!\n", count_hold++);
			break;
			case GESTURE_DRAG:
				printf("Fire Drag ----------%d!\n", count_hold++);
			break;
			case GESTURE_TAP:
				printf("Fire Tap ----------%d!\n", count_hold++);
			break;
			case GESTURE_TRANSFORM:
				printf("Fire Transfrom ----------%d!\n", count_hold++);
			break;
		default:
			break;
	}




}
