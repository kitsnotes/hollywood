.POSIX:

all: requirements/html/index.html script/html/index.html vision/html/index.html

image:
	docker build . -t cookbook
site:
	docker run -u$(shell id -u):$(shell id -g) -v$(CURDIR):/tmp -w /tmp --rm -it cookbook make
	rm -fr out
	mkdir out
	cp index.html out
	mv requirements/html out/requirements
	mv script/html out/script
	mv vision/html out/vision

requirements/html/index.html: requirements/*.xml master.xsl pretty.css
	@make -C requirements

script/html/index.html: script/*.xml master.xsl pretty.css
	@make -C script

vision/html/index.html: vision/*.xml master.xsl pretty.css
	@make -C vision

upload-all:
	@make -C requirements upload
	@make -C script upload
	@make -C vision upload
