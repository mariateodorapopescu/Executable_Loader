First I checked if it really isn't the SIGSEGV signal, the one that gives the Segmentation Fault, and if it isn't, the default handler is called.

I checked if there is a reliable error address, and if there is not, the default handler is called. If these checks have passed, I take the address of the error and if it is null, then the default handler is called again. Otherwise, I take the start address of a segment and the end address and try to check if the address that gave the error is between these two addresses that describe a segment/length of a segment.

If this happens, I mark the check variable ok with 1 and store the position of the segment in the variable segm_x.

If it was possible to fit the error address in a segment, then I try to find the page where this address exists. If so, I check whether or not that page is in the bss segment and store the page number in the pg_x variable.

The existence in the bss segment is marked again by means of a variable bsss, initially null.

I made a kind of matrix, or rather vector of vectors where I check for each page of a segment if it is mapped or not.
If the page is unmapped, i.e. if there is 0 on the row and column of the corresponding matrix, then the page is unmapped. Basically, initially all pages are unmapped, which is why the entire array is null.

If it is unmapped, then I check for cases where the page exists or not in the bss segment. I am going to find out the size that I have to map in memory, which is taken in several cases, depending on the place of the page that gave the error. I start with the case that the page with the error is not in this segment, so I check if the file size is less than the size of a page, and based on that I take the size I need to map. Otherwise, if it is still part of bss, I have several cases: when the size up to the current page is smaller than that of the file, then it's a normal mapping; if i bigger then it is zeroed.

If they are equal, it is read from the executable file, when the size from the segment to the current page is smaller than the file size. A difference is made and it is checked if this difference is smaller or larger than the size of an actual page. For the strictly greater case I know I have to map a page fixed (no more) and instead for the equal case I have two other cases where I check whether or not it's less than the size up to to the next page. Depending on this, I adjust the size I use to map.

Also in the case of equality between the size up to the current page and that of the file, I mark the fact that I have to read from the file via the reed variable. At the end of it all, until I get out of the case where I check the existence of the page with the error in the bss segment, I mark a variable that indicates whether or not to put memory zeros, the memm variable, especially since there are uninitialized, null data in the bss area.

After I found out the size to map, I put the page in memory (do mmap). If the mapping fails, I display the function name where the error occurs and exit the program. Otherwise, I mark in the matrix of pages for each segment the fact that the respective page has been mapped.

Then I start the other checks for which I took so many check variables: if zeroization is needed, that is, the memm variable is one, then I use memset to put zeros in the memory, in the remaining place between mem_size and file_size (mem_size is more big). For the case where I have to read, i.e. when the error falls in the middle of a page, I read from the executable file by placing the cursor from where I mapped, at the offset from which the respective segment starts + the size of the pages up to the page where it occurred the error and read until the error address is reached, which in this case falls in the middle of the page.

Finally, if the page has not been mapped, I put the permissions with the help of the mprotect function in the place where I mapped, the size of a page.

Likewise, if the mprotect functions, the read function, and the lseek function give errors, I display the name of the function that gave the error and exit the program.

Thus, I exit the if that checked that the page is unmapped. If it is, the default handler is called.

Likewise, when I exit from checking that the page is among the segments, if it does not belong to any, the default handler is called.

However, this is not the only case where I had to modify the given file to make it fully functional. In the so_init_loader initialization function I also had to put an empty set of signals to which I later added the SIGSEGV signal and modified the handling of this signal by adding the default handler at the end, which I previously initialized statically global.

In the function that serves as main, so_execute, we extracted the size of a page to be able to find out page number from each segment in the segment vector of the executable structure, this is to allocate the array of mapped and unmapped pages, I opened the executable file with read rights, and finally, after the executable's start function was called, I closed the file.

This assignment was made with the help of the old courses and labs provided in the useful resources section of the assignment requirement on ocw, labs four and six being very helpful. Another major contribution to the theme was the man pages of the mmap, lseek, read, mprotect, memset functions.
