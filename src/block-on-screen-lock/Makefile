block-until-screen-lock: block_until_screen_lock.c
	clang $< -o $@ -framework CoreFoundation  -framework IOKit

install: ../../block-until-screen-lock

../../block-until-screen-lock: block-until-screen-lock
	cp $< ../..

clean:
	rm -f ../../block-until-screen-lock block-until-screen-lock
