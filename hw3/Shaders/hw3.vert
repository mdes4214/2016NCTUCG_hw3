#version 440

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_norm;
layout(location = 2) in vec2 in_texcoord;

layout(binding = 0) uniform sampler2D texture;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat4 View;
uniform vec3 lightPos_in;
uniform int mode;

uniform vec3 objectColor = vec3(0.8, 0.8, 0.8);
uniform vec3 amColor = vec3(1.0, 1.0, 1.0), diffColor = vec3(1.0, 1.0, 1.0), specColor = vec3(1.0, 1.0, 1.0);
uniform float shiness = 40.0;
uniform float amCoe = 0.1f;
uniform float specCoe = 1.0f;

out vec2 texcoord;
out vec3 lightPos;
out vec3 normal;
out vec3 eyePos;
flat out vec3 result_flat;
out vec3 result;

void main() {
	texcoord = in_texcoord;
	vec4 texColor = texture2D(texture, in_texcoord);
	lightPos = vec3(View * vec4(lightPos_in, 1.0));
	mat3 normalMatrix = mat3(transpose(inverse(ModelView)));
	normal = normalize(normalMatrix * in_norm);
	eyePos = vec3(ModelView * vec4(in_pos, 1.0));

	if(mode == 1 || mode == 3){ // Flat shading & Gouraud shading
		vec3 lightDir = normalize(lightPos - eyePos);
		vec3 view = normalize(-eyePos);
		vec3 reflectDir = reflect(-lightDir, normal);

		vec3 ambient = amCoe * amColor;

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * diffColor;

		float spec = pow(max(dot(reflectDir, view), 0.0), shiness);
		vec3 specular = specCoe * spec * specColor;

		//result_flat = (ambient + specular) * objectColor + diffuse * texColor.xyz;
		//result = result_flat;
		result_flat = ambient * vec3(0.4, 0.4, 0.4) + diffuse * texColor.xyz;
		result = ambient * vec3(0.4, 0.4, 0.4) + specular * objectColor + diffuse * texColor.xyz;
	}

	gl_Position = Projection * ModelView * vec4(in_pos, 1.0);
}