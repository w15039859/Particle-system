/* Pong Solution:
   Sample solution for the 'Pong' assignment.

   Dr Alun Moon
   Spring 2017
*/

/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* hardware platform libraries */
#include <display.h>
#include <mbed.h>

#include <node.h>

/* SECTION: Graphics related code follows ------------------------------------*/
Display *lcd = Display::theDisplay();

/* Setup Double-buffering:
    The LCD display object uses malloc internally, to get enougth memory for
    3 frame buffers.  The display object renders into the buffer supplied
    through setFb, it is returned through getFb.
    The LPC4088 has an LCD driver that displays the contents of the buffer
    supplied through the UPBASE register.
    This function generates a pointer to the second buffer, and sets the
    display to use one, and the driver to use the other.
*/
void init_double_buffering(void)
{
    uint16_t *buffer = lcd->getFb();
    uint16_t *frame2 = buffer+(480*272);
    LPC_LCD->UPBASE = (uint32_t)frame2;
}

/* swap the frame buffers used by the Display GFX library
    and the LCD driver
*/
void swap_double_buffers(void)
{
    unsigned int b;
    b = LPC_LCD->UPBASE;
    LPC_LCD->UPBASE = (uint32_t)lcd->getFb();
    lcd->setFb((uint16_t *)b);
}


/* Draw a list of particles */
void drawparticles(struct particle *lst)
{
    /* while there are particles to draw */
    while(lst) {
        lcd->fillCircle(lst->x,lst->y, 1, RED);
        lst = lst->next;/* itterate along the list */
    }
}

/* Initialise the properties of a new particle */
int randrange(int from, int to)
{
    int range = to-from;
    return rand()%range + from;
}
float radians(float a)
{
    const float pi = 3.141592;
    return a * pi /180.0f; 
}
void strike(struct particle *r)
{
    r->x = 240;
    r->y = 130;
    r->v_x = randrange(-20,20);
    r->v_y = -randrange(15,25);
    r->ttl = 20; /* how long to live (s) */
}

/* update the position of each particle in a list
    of particles
*/
const float g=10;
const float Dt = 0.01;
void update(struct particle *l)
{
    for( ; l ; l = l->next ) {
        l->x += l->v_x * Dt; 
        l->y += l->v_y * Dt;
        
        l->v_y += g * Dt;
        
        if( l->y > 270 ){
            l->y = 270;
            l->v_y *= -0.7;
        }
        if( l->x<0 || 480<l->x ) l->ttl=0;
        l->ttl -= Dt;
        /* before moving on to the next element,
           check if it is still valid (has steps left)
           I have to do  this now, so I can remove the
           element from the linked-list
        */
        if( l->next->ttl<=0 ) {
            struct particle *expired = l->next;
            l->next = l->next->next;
            freenode(expired);
        }
    }
}

struct particle *active = NULL;
void draw(void)
{
        /* with Double-buffering I can just clear the screen
           and draw everything fresh again
        */
        lcd->fillScreen(BLACK);
        drawparticles(active);

        swap_double_buffers();
}

void particle_system(void)
{
        struct particle *spark = allocnode();
        if(spark) {
            /* put the new particle at the front of the list */
            spark->next = active;
            active = spark;
            strike(spark);
        }

        update(active);
}

Ticker drawing;
Ticker particles;
int main()
{
    initialise_heap();
    init_double_buffering();

    drawing.attach(draw, 0.025); 
    particles.attach(particle_system, Dt);
    while(1) {
        wait(20);
    }
}
