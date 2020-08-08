// Effect.fx

matrix pos_transform_mtx;
matrix norm_transform_mtx;

// A domain and range in the plane.
float Xmin, Xmax;
float Ymin, Ymax;

// This is the type of Julia set we want.
float2 C;

struct VS_INPUT
{
	vector pos			: POSITION;
	vector norm			: NORMAL0;
	float2 texcoords	: TEXCOORD0;
};

struct VS_OUTPUT
{
	vector pos			: POSITION;
	vector color		: COLOR;
	float2 texcoords	: TEXCOORD0;	
};

texture Tex0, Tex1;
sampler S0 = sampler_state
{
	Texture = (Tex0);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
sampler S1 = sampler_state
{
	Texture = (Tex1);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

float fade_factor;

VS_OUTPUT VS_Main( VS_INPUT vtx_in )
{
	VS_OUTPUT vtx_out = ( VS_OUTPUT )0;
	
	vtx_in.pos.w = 1.0f;
	vtx_out.pos = mul( vtx_in.pos, pos_transform_mtx );
	vtx_in.norm.w = 0.0f;
	vtx_in.norm = mul( vtx_in.norm, norm_transform_mtx );
	
	vtx_out.color.r = 1.0f;
	vtx_out.color.g = 1.0f;
	vtx_out.color.b = 1.0f;
	vtx_out.color.a = 0.0f;
	
	vtx_out.texcoords = vtx_in.texcoords;
	
	return( vtx_out );
}

struct PS_INPUT
{
	vector color		: COLOR;
	float2 texcoords	: TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

PS_OUTPUT PS_JuliaMain( PS_INPUT pix_in )
{
	PS_OUTPUT pix_out = ( PS_OUTPUT )0;
	
//	pix_out.color.r = pix_in.texcoords.x;
//	pix_out.color.g = pix_in.texcoords.y;
//	pix_out.color.b = pix_in.texcoords.x * pix_in.texcoords.y;
	pix_out.color.a = 0.0f;
	
	float2 Z;
	float tmp;
	int k;
	int esc = 0;
	
	Z.x = Xmin + pix_in.texcoords.x * (Xmax - Xmin);
	Z.y = Ymin + (1.0f - pix_in.texcoords.y) * (Ymax - Ymin);
	
#define ORBITS		40
	
	for( k = 0; k < ORBITS && !esc; k++ )
	{
		tmp = Z.x;
		Z.x = Z.x * Z.x - Z.y * Z.y + C.x;
		Z.y = 2.0f * tmp * Z.y + C.y;
		
		if( Z.x * Z.x + Z.y * Z.y > 4.0f )		
			esc = 1;
	}
	
	if( esc != 1 )
	{
		// The complex number is approximately a member of the set.
		pix_out.color.r = 0.0f;
		pix_out.color.g = 0.0f;
		pix_out.color.b = 0.0f;
	}
	else
	{
		// The complex number is not a member of the set.
		float scale = float(k) / float(ORBITS - 1);
		
#if 0	// This gives us a grey-scale image.
		pix_out.color.r = scale;
		pix_out.color.g = scale;
		pix_out.color.b = scale;
#else
		float2 texcoords = float2( scale, 0.0f );
		pix_out.color = tex1D( S1, texcoords );
#endif
	}
	
	return( pix_out );
}

PS_OUTPUT PS_FadeMain( VS_INPUT pix_in )
{
	PS_OUTPUT pix_out = ( PS_OUTPUT )0;
	
	float4 color0, color1;
	color0 = tex2D( S0, pix_in.texcoords );
	color1 = tex2D( S1, pix_in.texcoords );
	
	pix_out.color = color0 + (color1 - color0) * fade_factor;
	
	return( pix_out );
}

technique MyTech0
{
	pass P0
	{
		vertexshader = compile vs_3_0 VS_Main();
		pixelshader = compile ps_3_0 PS_FadeMain();
		
		Lighting = false;
	}
}

technique MyTech1
{
	pass P0
	{
		vertexshader = NULL;
		pixelshader = NULL;
		
		FillMode = WIREFRAME;
	}
}

technique MyTech2
{
	pass P0
	{
		vertexshader = compile vs_3_0 VS_Main();
		pixelshader = compile ps_3_0 PS_JuliaMain();
	}
}

// endof Effect.fx