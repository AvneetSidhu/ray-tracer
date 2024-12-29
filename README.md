
![image](https://github.com/user-attachments/assets/fe0cd870-2518-4c6e-b7a3-2fd90a5fa61c)


Implemented multithreading by rendering the image in chunks based on the number of threads available at the time.

Created an image buffer using a vector that holds rows of colors that represent pixels. Each thread writes to the image buffer in parallel and fills the (i,j) pixel with its color.

