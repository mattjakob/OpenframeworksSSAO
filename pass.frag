
uniform sampler2D textureSource;

void main()
{
	vec4 color = vec4(0.0);
	color = texture2D( textureSource, gl_TexCoord[0].st  );
	vec4 inv = vec4(1, 1, 1, 1);
	gl_FragColor = inv - color;

}