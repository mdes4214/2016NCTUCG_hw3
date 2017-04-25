#version 440

layout(binding = 0) uniform sampler2D texture;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat4 View;
uniform vec3 lightPos_in;
uniform int mode;

uniform vec3 objectColor;
uniform vec3 amColor, diffColor, specColor;
uniform float shiness;
uniform float amCoe;
uniform float specCoe;

in vec2 texcoord;
in vec3 lightPos;
in vec3 normal;
in vec3 eyePos;
flat in vec3 result_flat;
in vec3 result;

out vec4 outColor;

void main() {
	vec4 texColor = texture2D(texture, texcoord);

	if(mode == 0) // default
		outColor = texColor;
	else if(mode == 1) // Flat shading
		outColor = vec4(result_flat, 1.0f);
	else if(mode == 3) // Gouraud shading
		outColor = vec4(result, 1.0f);
	else if(mode == 4){ // Gooch shading
		vec3 view = normalize(-eyePos);
		
		if(dot(normal, view) <= 0.2){ // silhouette
			outColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else{
			vec3 lightDir = normalize(lightPos - eyePos);
			vec3 reflectDir = reflect(-lightDir, normal);
			float alpha = 0.5;
			float beta = 0.5;
			vec3 cool = vec3(0.0, 0.0, 0.8);
			vec3 warm = vec3(0.6, 0.6, 0.1);

			vec3 ambient = amCoe * amColor;

			float diff = max(dot(normal, lightDir), 0.0);
			vec3 cdiff = cool + alpha * diffColor;
			vec3 wdiff = warm + beta * diffColor;
			vec3 diffuse = (1.0 + diff) / 2.0 * wdiff + (1.0 - (1.0 + diff) / 2.0) * cdiff;

			float spec = pow(max(dot(reflectDir, view), 0.0), shiness);
			vec3 specular = specCoe * spec * specColor;

			//vec3 color = (ambient + specular) * objectColor + diffuse * texColor.xyz;
			vec3 color = (ambient + diffuse + specular) * objectColor;
			outColor = vec4(color, 1.0f);
		}
	}
	else{ // Phong shading
		vec3 lightDir = normalize(lightPos - eyePos);
		vec3 view = normalize(-eyePos);
		vec3 reflectDir = reflect(-lightDir, normal);

		vec3 ambient = amCoe * amColor;

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * diffColor;

		float spec = pow(max(dot(reflectDir, view), 0.0), shiness);
		vec3 specular = specCoe * spec * specColor;

		//vec3 color = (ambient + specular) * objectColor + diffuse * texColor.xyz;
		vec3 color = ambient * vec3(0.4, 0.4, 0.4) + specular * objectColor + diffuse * texColor.xyz;
		outColor = vec4(color, 1.0f);
	}
}