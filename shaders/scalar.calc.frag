#version 120

uniform float width;
uniform float height;

uniform sampler2D tex;

// add 1/w and 1/h to get neightbor
//*
float texdx = 1.0 / width;
float texdy = 1.0 / height;

float gol(float sum, float self) {
	// Rewrite as branchless
	if (sum < 1.5) {
		// Loneliness
		return 0.0;
	} else if (sum > 3.5) {
		// Overcrowded
		return 0.0;
	} else if (sum < 3.5 && sum > 2.5) {
		// If exactly three, stay alive or come to life
		return 1.0;
	} else if (self == 1.0) {
		// If two, then stay alive,
		return 1.0;
	} else {
		// But do not come to life
		return 0.0;
	}
}

void main () {
	vec2 coord = gl_TexCoord[0].xy;
	
	float texx = coord.x;
	float texy = coord.y;
		
	vec4 lu = texture2D(tex, vec2(texx - texdx, texy + texdy));
	vec4 mu = texture2D(tex, vec2(texx        , texy + texdy));
	vec4 ru = texture2D(tex, vec2(texx + texdx, texy + texdy));
	vec4 lm = texture2D(tex, vec2(texx - texdx, texy        ));
	vec4 mm = texture2D(tex, vec2(texx        , texy        ));
	vec4 rm = texture2D(tex, vec2(texx + texdx, texy        ));
	vec4 ld = texture2D(tex, vec2(texx - texdx, texy - texdy));
	vec4 md = texture2D(tex, vec2(texx        , texy - texdy));
	vec4 rd = texture2D(tex, vec2(texx + texdx, texy - texdy));
	
	float g = gol(lu.r + mu.r + ru.r + lm.r + rm.r + ld.r + md.r + rd.r, mm.r);
	gl_FragColor = vec4(g, 0.0, 0.0, 1.0);
}