_NOTE: For dumbrleplayer and smartrleplayer, I have **purposely** made them to wait for 2 seconds_
_before exiting (it seemed cleaner than having them wait forever upon completion or simply exiting_
_them as soon as they've processed the frames). This is done with an SDL_Delay call (it's not_
_hanging!)._


##Source and header files:

###util.c:
Contains number of utility functions used by all four programs, such as file handling
(including checking incorrect file types), and error handling.

###util-SDL.h, util.h:
Separates the function prototypes for util.c functions by those that implement SDL libraries and
those that don't. Useful as it allows programs that don't use SDL libraries (dumrletoppm) but also
include util.c functions to not have to include SDL.h and link in SDL libs to compile properly.
Also defines mutually-used constants.

###util-dumbrle.c:
Contains DUMBRLEv1 processing functions that handle processing of frames and individual channels
for an SDL_Surface.

###firstframeview.c, dumbrleplayer.c:
Note that these programs are almost the same and implement the same functions in almost the same way
(defined in util-dumbrle.c). firstframeview simply exits after the first frame is processed, while
dumbrleplayer continues to process all the frames.
This wasn't intentional, I just realised that after completing stage 2, I'd essentially done
everything needed to make stage 3 work (apart from adding an additional CLI arg + SDL_Delay() call).

###util-HSV.c, util-HSV.h:
Extensively modifies existing algorithms for converting between RGB and HSV pixels, originally
written by Jef Poskanzer and included in libppm in netpbm-10.61-02. I have permission to modify
these as included at the start of util-HSV.c, although apart from Poskanzer's underlying algorithms,
all code and comments are my own.
The reasons for doing this:
- The Xubuntu distribution that this assignment is aimed to work on had a far older version of
libppm in its repositories, which didn't support converting libppm pixels to HSV.
- Even if I included a more up to date version of libppm with the assignment, I had already
implemented my video player using an SDL_Surface which uses Uint32 to represent pixels. This requires
multiple iterations through every pixel to convert from Uint32 to libppm pixel to HSV, alter HSV
values and convert all the way back, which I didn't want to put in my code. Re-implementing the
existing algorithms to make functions that work with an SDL_Surface seemed like the best way to go
(and it was quite fun).
__More info is documented in util-HSV.c__

###smartrleplayer.c:
All display options, brightness, contrast and saturation, should work as specified in assignment
spec.
