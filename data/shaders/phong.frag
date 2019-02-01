#version 330

//varyings and out color
in vec2 v_uv;
in vec3 v_normal;
in vec3 v_light_dir;
in vec3 v_cam_dir;
in vec3 v_vertex_world_pos;
out vec4 fragColor;

//basic material uniforms
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_specular_gloss;

//texture uniforms
uniform sampler2D u_diffuse_map;

//light structs and uniforms
struct PointLight {
	vec3 position;
	vec3 color;
};
const int MAX_LIGHTS = 8;
uniform PointLight lights[MAX_LIGHTS];
uniform int u_num_lights;


void main(){

	vec3 diffuse_map = texture(u_diffuse_map, v_uv).xyz;

	//ambient light
	vec3 final_color = u_ambient * diffuse_map;
	
	//loop lights
	for (int i = 0; i < u_num_lights; i++){

		vec3 L = normalize(lights[i].position - v_vertex_world_pos); //to light
		vec3 N = normalize(v_normal); //normal
		vec3 R = reflect(-L,N); //reflection vector
		vec3 V = normalize(v_cam_dir); //to camera

		//diffuse color
		float NdotL = max(0.0, dot(N, L));
		vec3 diffuse_color = NdotL * diffuse_map * u_diffuse * lights[i].color;

		//specular color
		float RdotV = max(0.0, dot(R, V)); //calculate dot product
		RdotV = pow(RdotV, u_specular_gloss); //raise to power for glossiness effect
		vec3 specular_color = RdotV * lights[i].color * u_specular;

		//final color
		final_color += diffuse_color + specular_color;
	}

	fragColor = vec4(final_color, 1.0);
}