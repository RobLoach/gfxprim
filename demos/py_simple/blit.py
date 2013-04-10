#!/usr/bin/env python

import sys
from time import sleep

import gfxprim.core as core
import gfxprim.loaders as loaders
import gfxprim.backends as backends
import gfxprim.input as input

class Ball:
    def __init__(self, x, y, dx, dy, path, bg_img):
        self.ball = loaders.Load(path)
        assert(self.ball)

        self.x = x
        self.y = y
        self.dx = dx
        self.dy = dy

        self.bg_img = bg_img

    def draw(self, bk):
        self.ball.Blit(0, 0, bk.context, self.x, self.y, self.ball.w, self.ball.h)

    def move(self, bk):
        old_x = self.x;
        old_y = self.y;

        self.bg_img.Blit(old_x, old_y, bk.context, old_x, old_y, self.ball.w, self.ball.h)

        self.x += self.dx
        self.y += self.dy

        if (self.x <= 0 or self.x >= self.bg_img.w - self.ball.w):
            self.dx = -self.dx

        if (self.y <= 0 or self.y >= self.bg_img.h - self.ball.h):
            self.dy = -self.dy

        self.ball.Blit(0, 0, bk.context, self.x, self.y, self.ball.w, self.ball.h)
        bk.UpdateRect(min(old_x, self.x), min(self.y, old_y),
                      max(old_x, self.x) + self.ball.w - 1,
                      max(old_y, self.y) + self.ball.h - 1)

def main():
    if len(sys.argv) != 2:
        print("Takes an image as an argument")
        sys.exit(1)

    # Load Backgroudn Image and ball sprite
    bg = loaders.Load(sys.argv[1])
    assert(bg)
    
    ball1 = Ball(bg.w//2, bg.h//2, -3, -3, 'ball_red.png', bg)
    ball2 = Ball(bg.w//2, bg.h//2, -2,  3, 'ball_green.png', bg)
    ball3 = Ball(bg.w//2, bg.h//2,  2, -3, 'ball_blue.png', bg)

    # Create X11 window
    bk = backends.BackendX11Init(None, 0, 0, bg.w, bg.h, sys.argv[1], 0)
    assert(bk)
    bg.Blit(0, 0, bk.context, 0, 0, bg.w, bg.h)

    bk.Flip()

    # Event loop
    while True:
        
        while True:
            ev = bk.PollEvent()

            if (ev is None):
                break

            input.EventDump(ev)

            if (ev.type == input.EV_KEY and ev.val.val == input.KEY_ESC):
               sys.exit(0)
            elif (ev.type == input.EV_SYS):
               if (ev.code == input.EV_SYS_QUIT):
                   sys.exit(0)
       
        sleep(0.005)

        ball1.move(bk);
        ball2.move(bk);
        ball3.move(bk);

if __name__ == '__main__':
    main()
