obj = malloc.o

exe: $(obj)  bucket.h malloc.h 
	cc -o exe  malloc.o malloc.h
	@echo "make has been completed."


malloc.o:

clean:
	rm exe $(obj) 
	@echo "exe, $(obj) files are removed."

