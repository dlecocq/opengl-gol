#version 110

uniform sampler2D tex;

void main () {
	float value = abs(1.0 - texture2D(tex, gl_TexCoord[0].xy).r);
	
	gl_FragColor = vec4(value, value, value, 1.0);
}