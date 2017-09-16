# ImageQuilting
An implementation of Efros and Freeman's algorithm for texture synthesis and transfer.

# User Instructions
Launch the executable from the command line. It is called “Image Quilting” and is in the “Executable/Release” folder, along with sample image files.

For texture synthesis the arguments are:
<source_image_path> <block_size> <border_size> <randomness> <width> <height>
block_size is the width and height of a "block", a chunk of the source image that will be extracted and pasted elsewhere
border_size is the number of pixels that blocks will overlap on their borders
randomness is the size of the pool of optimal blocks that can be randomly selected from. choosing 1 will make the algorithm always place the best block, 2 will make it randomly choose between the best 2 blocks, etc.

example:
$ ./”Executable/Release/Image Quilting” Images/rice.ppm 10 3 2 400 400


For texture transfer the arguments are:
<source_image_path> <block_size> <border_size> <randomness> <target_image_path>

example:
$ ./”Executable/Release/Image Quilting” Images/fakeGrass.ppm 10 3 2 trump.ppm


Note: All image files must be ppm or bpm format.
