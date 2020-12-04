#version 450 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

// Texture sampler
uniform sampler2D ourTexture1;

// Vari�veis uniform com par�metros para os exerc�cios
uniform vec3 corColorizadora;

void main()
{
    //O comando texture � da pr�pria GLSL, e busca a cor do fragmento com a coord de 
    //textura fornecida
    color = texture(ourTexture1, TexCoord);

    //LEMBRE-SE: aqui o valor dos canais de cor est� NORMALIZADO (entre 0.0 e 1.0)
    
    // 1c) - Exibir imagem colorizada
	corSomada = vec4(corColorizadora, 0.0); // Alpha 0.0 para n�o alterar como j� �
	color = cor + corSomada;
}