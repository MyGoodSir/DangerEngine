#pragma once

#include "Danger.h"
#include "util/typealias.h"
#include "util/Logger.h"
#include "shader.h"
#include "mesh.h"

#include <vector>

//framebuffer texture data
struct FBTexture {
	GLenum format, precision, datatype;
	uint attachment_number;
	uint handle;
};
//framebuffer data
struct Framebuffer {
	uint handle;
	uint width, height;
	uint num_attachments;
	std::vector<FBTexture> attachments;
	uint h_rbo;
};



class FramebufferManager {
public:
	static void bind(Framebuffer &fb) {
		glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
	}
	static void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	static void deleteFramebuffer(Framebuffer &fb) {
		glDeleteFramebuffers(1, &fb.handle);
	}
	//is this a complete framebuffer
	static bool check_status() {
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}
	//create new framebuffer
	static Framebuffer create(uint width, uint height) {
		Framebuffer fb{};
		glGenFramebuffers(1, &fb.handle);
		fb.width = width;
		fb.height = height;
		return fb;
		
	}
	//create new framebuffer texture
	static void create_fbtexture(Framebuffer& fb, FBTexture tex) {
		tex.attachment_number = fb.num_attachments;
		fb.num_attachments++;
		glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
		glGenTextures(1, &tex.handle);
		glBindTexture(GL_TEXTURE_2D, tex.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, tex.precision, fb.width, fb.height, 0, tex.format, tex.datatype, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + tex.attachment_number, GL_TEXTURE_2D, tex.handle, 0);
		fb.attachments.push_back(tex);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

	//draw to all framebuffer attachments
	static void draw_target_all(Framebuffer& fb) {
		glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
		uint* a_arr = new uint[fb.num_attachments];
		for (uint i = 0; i < fb.num_attachments; i++ ) {
			a_arr[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(fb.num_attachments, a_arr);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	//generate a renderbuffer for the specified framebuffer
	static void gen_renderbuffer(Framebuffer& fb) {
		glBindFramebuffer(GL_FRAMEBUFFER, fb.handle);
		uint rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fb.width, fb.height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		if (
			(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) ){
			DGR_LOG_ERROR("Framebuffer not complete!");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};