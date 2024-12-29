![image](https://github.com/user-attachments/assets/426ddfcd-2d6c-44c2-8a04-4236ddc793e1)

Implemented multithreading by rendering the image in chunks based on the number of threads available at the time.

Created an image buffer using a vector that holds rows of colors that represent pixels. Each thread writes to the image buffer in parallel and fills the (i,j) pixel with its color.

