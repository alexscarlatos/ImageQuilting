# ImageQuilting
An implementation of Efros and Freeman's algorithm for texture synthesis and transfer.

## User Instructions
Launch the executable from the command line. It is called “Image Quilting” and is in the Executable/Release folder. Sample images are provides in the Images folder.

Note: the build provided is for Unix machines

### Texture Synthesis
Usage: `<source_image_path> <block_size> <border_size> <randomness> <new_width> <new_height>`
<br/>
*block_size* is the width and height of a "block", a chunk of the source image that will be extracted and pasted elsewhere
<br/>
*border_size* is the number of pixels that blocks will overlap on their borders
<br/>
*randomness* is the size of the pool of optimal blocks that can be randomly selected from. choosing 1 will make the algorithm always place the best block, 2 will make it randomly choose between the best 2 blocks, etc.

Ex: `$ ./”Executable/Release/Image Quilting” Images/rice.ppm 10 3 2 400 400`

### Texture Transfer
Usage: `<source_image_path> <block_size> <border_size> <randomness> <target_image_path>`

Ex: `$ ./”Executable/Release/Image Quilting” Images/fakeGrass.ppm 10 3 2 potato.ppm`

Note: All image files must be ppm or bpm format.
