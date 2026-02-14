@all: ./builddir/main

./builddir/main: ./builddir
	ninja -C ./builddir

./builddir: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

install: ./builddir/main
	install -d $(DESTDIR)/usr/bin
	install ./builddir/main $(DESTDIR)/usr/bin/main

run: ./builddir/main
	./builddir/main
