#pragma once

#include "Scene.h"


/*\
 * Sampler Objects can be manipulated. they are usually bundled with the texture
 * Renderbuffer Objects are used with a framebuffer and are a texture that can be rendered to offscreen
 * Framebuffer: color, depth, and stencil buffers <- all are either RBOs or attached
 * Program Pipeline Objects contain a separate program binding point for each programmable rendering stage
 * Vertex Array Objects: represent a collection of _sets_ of vertex attributes. each set has a buffer obj and certain formats and component counts
 * Transform Feedback Objects: used to capture attributes of vertices of transformed primitives (yoink the data after the vert, tessilation, and geom shaders)
 * Query Objects: return info about the processing of a sequence of GL commands. eg. # of primitives processed by drawing cmds, # of prims written to TFO, # of pixels that pass the depth test, amount of time to process cmds
 * Sync Object: synchronization primitive
 * Shader Storage Block: Usually shader defined buffers that are similar to uniforms
 *
 *
 * Buffer Types:
 *		-Array Buffer: vertex attribs
 *		-Atomic Counter Buffer: atomic storage...
 *		-Copy Read/Write Buffers: copy
 *		-Dispatch Indirect Buffer: Indirect compute dispatch commands
 *		-Draw Indirect Buffer: Indirect Command Arguments
 *		-Element Array Buffer: Vert array indices
 *		-Transform Feedback Buffer: yoink the data before its a pixels
 *		-Uniform Buffer: Uniform Block Storage
 *
 * Buffer Commands:
 *		-GenBuffers, IsBuffer, BindBuffer
 *		-BindBufferRange: bind a buffer to an indexed target, like a Uniform Buffer, to populate its components
 *		-BindBufferBase: bind an indexed buffer, but bind the whole thing.
 *		-BufferStorage: create the data store of a buffer or overwrite the existing one. will unmap your buffer first.
 *		-NamedBufferStorage: same as above, but DSA
 *		-MapBuffer: map the buffer memory to the virtual address space 
 *		-UnmapBuffer:undo the above
 *		-BufferSubData: write to a portion of a buffers memory
 *		-Buffer<Named>Data: mutable storage that has one attribute from each of these categories:
 *								> STREAM (set once, use a few times), STATIC (set once, use many times), DYNAMIC (set much more than once, and use very often)
 *								> DRAW, READ, COPY
 *		
 *
 * BufferFlags:
 *		-DYNAMIC_STORAGE_BIT: contents of the buffer can be edited by BufferSubData
 *		-MAP_READ_BIT: maps buffer memory as rxx
 *		-MAP_WRITE_BIT: maps buffer memory as xwx (?)
 *		-MAP_PERSISTENT_BIT:(read or write is prereq) allow the buffer to be mapped while allowing GL to still fuck with it
 *		-MAP_COHERENT_BIT:(persistent bit is prereq) the above, but you get to fuck with the buffer too (you can do this without coherent bit if u flush the mapped buffer range after writing)
 *		-CLIENT_STORAGE_BIT: (gotta set all of the above first) actually store the data in main system memory
 *
 *
 * Program Pipelines:
 *		-GenProgramPipelines / CreateProgramPipelines: gen'd pipelines dont get state until they're bound. created pipelines get a default state immediately
 *		-UseProgram: binds all shaders associated with a program to the pipeline
 *		-UseProgramStages: binds specific stages of a program to the pipeline
 *		-for all inputs and outputs to be compatable between the shaders in a pipeline, they have to be, for all intents and purposes, declared identically
 *
 *
 * Uniforms:
 *		-Uniform Blocks: can not be comprised of atomic counters, images, samplers, or subroutine uniforms. named uniform blocks have a UBI
 *		-UniformBlockBinding: assigns the binding point of a uniform buffer object
 *		-Uniforms: set once before draw call and accessable by shaders. single uniforms that dont belong to a named block belong to the default uniform block
 *		-All members of a named uniform block declared with a "shared" or "std140" layout qualifier are considered active, even if they are not referenced in any shader in the program.
 *		-there is a max amount you can have of each uniform datatype
 *
 *
\*/


