#version 330 core

// Depth-only pass: the FBO has no color attachment, so nothing to write.
// gl_FragDepth is filled in automatically.
void main()
{
}
