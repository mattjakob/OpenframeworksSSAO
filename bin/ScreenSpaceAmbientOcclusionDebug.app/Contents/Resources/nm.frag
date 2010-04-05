
varying vec3 normal;

void main()
{
	gl_FragColor = vec4(normal.x, normal.y, normal.z, gl_FragCoord.z);
	//gl_FragColor = vec4( gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z,gl_FragCoord.z);

}

