/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <iostream>
#include <algorithm>
#include <string.h>

 


/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
    buffer_list.clear();
    aktiv_vertex = emptyID;
    aktiv_prog = emptyID;
    buf_id.clear();
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
	for(int i = 0; i<buffer_list.size();++i)
	{
		if(buffer_list[i] != NULL)
			delete [] (char*)buffer_list[i];
	}
	buffer_list.clear();
	for(int i = 0; i<vertex_list.size();++i)
	{
		if(vertex_list[i] != NULL)
			delete vertex_list[i];
	}
	vertex_list.clear();
	
	for(int i = 0; i<program_list.size();++i)
	{
		if(program_list[i] != NULL)
			delete program_list[i];
	}
	program_list.clear();



	
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
    BufferID id;
    if (buf_id.size() == 0)
    {
        id = buffer_list.size();
        void* prvok = operator new [] (size);
        buffer_list.push_back(prvok);
    }
    else
    {
            id = buf_id.back();
            buf_id.pop_back();
            void* prvok = operator new[](size);
            buffer_list[id] = prvok;
    }
    
  return id; 
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
  
    delete [] (char*)buffer_list[buffer];
    buffer_list[buffer] = NULL;
    buf_id.push_back(buffer);
        
    
    
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
    // riesenie s posunom pri ukazovateli na void: https://stackoverflow.com/questions/6449935/increment-void-pointer-by-one-byte-by-two
   
    memcpy(static_cast<char*>(buffer_list[buffer]) + offset, data, size);
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  // riesenie s posunom pri ukazovateli na void: https://stackoverflow.com/questions/6449935/increment-void-pointer-by-one-byte-by-two
    memcpy( data, static_cast<char*>(buffer_list[buffer]) + offset, size);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
    if (buffer == emptyID || buffer >= buffer_list.size())
    {
        return false;
    }
    else if(buffer_list[buffer] == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}   

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.
    ObjectID id; 
    
    if (ver_id.size() == 0)
    {
        id = vertex_list.size();
        vertex_list.push_back((tabulka*)new tabulka);
    }
    else
    {
        id = ver_id.back();
        ver_id.pop_back();
        vertex_list[id] = (tabulka*)new tabulka;
    }
    return id;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
    delete vertex_list[vao];
    vertex_list[vao] = NULL;
    ver_id.push_back(vao);
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
    vertex_list[vao]->hlavy[head].type = type;
    vertex_list[vao]->hlavy[head].stride = stride;
    vertex_list[vao]->hlavy[head].offset = offset;
    vertex_list[vao]->hlavy[head].buffer = buffer;
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
    vertex_list[vao]->index.type = type;
    vertex_list[vao]->index.buffer = buffer;
    vertex_list[vao]->ind = true;
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
    vertex_list[vao]->hlavy[head].enable = true;
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
    vertex_list[vao]->hlavy[head].enable = false;
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
    aktiv_vertex = vao;
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
    aktiv_vertex = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
    if (vao == emptyID || vao >= vertex_list.size())
    {
        return false;
    }
    else if (vertex_list[vao] == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
    ObjectID id;
  if (pro_id.size() == 0)
  {
      id = program_list.size();
      program_list.push_back((program*)new program);
  }
  else
  {
      id = pro_id.back();
      pro_id.pop_back();
      program_list[id] = (program*)new program;
  }
  
  return id;
}
  
/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>
    delete program_list[prg];
    program_list[prg] = NULL;
    pro_id.push_back(prg);
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
    program_list[prg]->vs = vs;
    program_list[prg]->fs = fs;
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
    
    program_list[prg]->atr_num.push_back((int)attrib);
    program_list[prg]->type.push_back((int)type);
    
    
    
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
    aktiv_prog = prg;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
    if (prg == emptyID || prg >= program_list.size())
    {
        return false;
    }
    else if (program_list[prg] == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
    program_list[prg]->premenne.uniform[uniformId].v1 = d;
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
    program_list[prg]->premenne.uniform[uniformId].v2 = d;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
    program_list[prg]->premenne.uniform[uniformId].v3 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
    program_list[prg]->premenne.uniform[uniformId].v4 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
    program_list[prg]->premenne.uniform[uniformId].m4 = d;
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
    //std::cout << width <<"    " <<height << std::endl;
    myframe.w = width;
    myframe.h = height;
    myframe.color.resize((4 * (int)width * (int)height));
    myframe.hlbka.resize((int)width * (int)height);
    //std::cout << myframe.color.size() << "    " << myframe.hlbka.size() << std::endl;
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
    if (myframe.w != width || myframe.h != height)
    {
        myframe.w = width;
        myframe.h = height;
        myframe.color.resize(4 * (int)width * (int)height);
        myframe.hlbka.resize((int)width * (int)height);
    }
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
  return  &myframe.color[0];
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
  return  &myframe.hlbka[0];
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
  return  myframe.w;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
  return  myframe.h;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
    int max =  myframe.h * myframe.w;
    //std::cout << max << "    " << myframe.hlbka.size() << std::endl;
    for (int i = 0; i < max; i++)
    {//std::cout << i << std::endl;
        if (r > 1)
            myframe.color[(i * 4)] = 255;
        else
            myframe.color[(i * 4)] = (int)(r * 255);
        if (g > 1)
            myframe.color[(i * 4 + 1)] = 255;
        else
            myframe.color[(i * 4 + 1)] = (int)(g * 255);
        if (b > 1)
            myframe.color[(i * 4 + 2)] = 255;
        else
            myframe.color[(i * 4 + 2)] = (int)(b*255);
        if (a > 1)
            myframe.color[(i * 4 + 3)] = 255;
        else
            myframe.color[(i * 4 + 3)] = (int) (a*255);
        myframe.hlbka[i] = 1.1f;
    }
}



void            GPU::drawTriangles         (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>


// zdroj informacii pre implementaciu rasterizacie https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation


    InVertex vrcholy;
    OutVertex vrcholy_out;
	vrcholy_out.gl_Position = glm::vec4(0, 0, 0, 0);
    std::vector<trojuhol> trojuholnik;
    trojuhol troj;
    
    for (uint32_t j = 0; j < nofVertices; j++)
    {
        
        if (vertex_list[aktiv_vertex]->ind)
        {
            int type = (int)vertex_list[aktiv_vertex]->index.type;
            ten_type = type;
            if (type == 1)
            {
                //uint8_t poradie;
            getBufferData(vertex_list[aktiv_vertex]->index.buffer, sizeof(uint8_t) * j,sizeof(uint8_t), &poradie1);
            vrcholy.gl_VertexID = (uint32_t)(poradie1);
            }
            else if (type == 2)
            {
                //uint16_t poradie;
                getBufferData(vertex_list[aktiv_vertex]->index.buffer,sizeof(uint16_t) * j,sizeof(uint16_t), &poradie2);
                vrcholy.gl_VertexID = (uint32_t)(poradie2);
            }
            else
            {
               // uint32_t poradie;

                getBufferData(vertex_list[aktiv_vertex]->index.buffer,sizeof(uint32_t) * j,sizeof(uint32_t), &poradie3);
                vrcholy.gl_VertexID = poradie3;
            }
        }
        else
        {
           vrcholy.gl_VertexID  = j;
        }
        
        for (uint32_t i = 0; i < maxAttributes; i++)
        {
            hlava hlava = vertex_list[aktiv_vertex]->hlavy[i];
            if (hlava.enable)
            {

                void* ciel = NULL;
                switch ((int)hlava.type)
                {
                case 1: ciel = &vrcholy.attributes[i].v1; break;
                case 2: ciel = &vrcholy.attributes[i].v2; break;
                case 3: ciel = &vrcholy.attributes[i].v3; break;
                case 4: ciel = &vrcholy.attributes[i].v4; break;
                }
                getBufferData(hlava.buffer, hlava.offset + hlava.stride * vrcholy.gl_VertexID, (int)hlava.type * sizeof(float), ciel);
            }
        }
        program_list[aktiv_prog]->vs(vrcholy_out, vrcholy, program_list[aktiv_prog]->premenne);
        troj.body[j % 3] = vrcholy_out;
       
        //trojuholnik[j / 3].body[j % 3] = vrcholy_out[j];
        if (-vrcholy_out.gl_Position.w > vrcholy_out.gl_Position.z)
        {
            clip_bod.push_back(j % 3);
        }
    
        

    
        if (j % 3 == 2)
        {
            if (clip_bod.size() == 3)
            {
                clip_bod.clear();
            }
            else if (clip_bod.size() == 1)
            {
                int bod1, bod2;
                switch (clip_bod[0])
                {
                case 0:bod1 = 1; bod2 = 2; break;
                case 1:bod1 = 0; bod2 = 2; break;
                case 2:bod1 = 0; bod2 = 1; break;
                }
                float citatel = -troj.body[clip_bod[0]].gl_Position.w - troj.body[clip_bod[0]].gl_Position.z;
                float t1 = (citatel) / (troj.body[bod1].gl_Position.w - troj.body[clip_bod[0]].gl_Position.w + troj.body[bod1].gl_Position.z - troj.body[clip_bod[0]].gl_Position.z);
                float t2 = (citatel) / (troj.body[bod2].gl_Position.w - troj.body[clip_bod[0]].gl_Position.w + troj.body[bod2].gl_Position.z - troj.body[clip_bod[0]].gl_Position.z);
                OutVertex vrchol1;
                trojuhol trojuholnik2;
                trojuholnik2.body[clip_bod[0]] = troj.body[bod1];
                trojuholnik2.body[bod1].gl_Position = troj.body[clip_bod[0]].gl_Position + t1 * (troj.body[bod1].gl_Position - troj.body[clip_bod[0]].gl_Position);
                trojuholnik2.body[bod2].gl_Position = troj.body[clip_bod[0]].gl_Position + t2 * (troj.body[bod2].gl_Position - troj.body[clip_bod[0]].gl_Position);
                for (uint32_t i = 0; i < maxAttributes; i++)
                {
                    
                    trojuholnik2.body[bod1].attributes[i].v1 = troj.body[clip_bod[0]].attributes[i].v1 + t1 * (troj.body[bod1].attributes[i].v1 - troj.body[clip_bod[0]].attributes[i].v1);
                    trojuholnik2.body[bod1].attributes[i].v2 = troj.body[clip_bod[0]].attributes[i].v2 + t1 * (troj.body[bod1].attributes[i].v2 - troj.body[clip_bod[0]].attributes[i].v2);
                    trojuholnik2.body[bod1].attributes[i].v3 = troj.body[clip_bod[0]].attributes[i].v3 + t1 * (troj.body[bod1].attributes[i].v3 - troj.body[clip_bod[0]].attributes[i].v3);
                    trojuholnik2.body[bod1].attributes[i].v4 = troj.body[clip_bod[0]].attributes[i].v4 + t1 * (troj.body[bod1].attributes[i].v4 - troj.body[clip_bod[0]].attributes[i].v4);
                    trojuholnik2.body[bod2].attributes[i].v1 = troj.body[clip_bod[0]].attributes[i].v1 + t2 * (troj.body[bod2].attributes[i].v1 - troj.body[clip_bod[0]].attributes[i].v1);
                    trojuholnik2.body[bod2].attributes[i].v2 = troj.body[clip_bod[0]].attributes[i].v2 + t2 * (troj.body[bod2].attributes[i].v2 - troj.body[clip_bod[0]].attributes[i].v2);
                    trojuholnik2.body[bod2].attributes[i].v3 = troj.body[clip_bod[0]].attributes[i].v3 + t2 * (troj.body[bod2].attributes[i].v3 - troj.body[clip_bod[0]].attributes[i].v3);
                    trojuholnik2.body[bod2].attributes[i].v4 = troj.body[clip_bod[0]].attributes[i].v4 + t2 * (troj.body[bod2].attributes[i].v4 - troj.body[clip_bod[0]].attributes[i].v4);
                }

                trojuholnik.push_back(trojuholnik2);
                troj.body[clip_bod[0]] = trojuholnik2.body[bod2];
                trojuholnik.push_back(troj);
                clip_bod.clear();
            }
            else if (clip_bod.size() == 2)
            {
                int bod = 3 - clip_bod[0] - clip_bod[1];
                float citatel = -troj.body[bod].gl_Position.w - troj.body[bod].gl_Position.z;
                float t1 = (citatel) / (troj.body[clip_bod[0]].gl_Position.w -troj.body[bod].gl_Position.w  + troj.body[clip_bod[0]].gl_Position.z - troj.body[bod].gl_Position.z);
                float t2 = (citatel) / (troj.body[clip_bod[1]].gl_Position.w - troj.body[bod].gl_Position.w + troj.body[clip_bod[1]].gl_Position.z - troj.body[bod].gl_Position.z);
                troj.body[clip_bod[0]].gl_Position = troj.body[bod].gl_Position + t1 * (troj.body[clip_bod[0]].gl_Position - troj.body[bod].gl_Position);
                troj.body[clip_bod[1]].gl_Position = troj.body[bod].gl_Position + t2 * (troj.body[clip_bod[1]].gl_Position - troj.body[bod].gl_Position);
                for (uint32_t i = 0; i < maxAttributes; i++)
                {

                    troj.body[clip_bod[0]].attributes[i].v1 = troj.body[bod].attributes[i].v1 + t1 * (troj.body[clip_bod[0]].attributes[i].v1 - troj.body[bod].attributes[i].v1);
                    troj.body[clip_bod[0]].attributes[i].v2 = troj.body[bod].attributes[i].v2 + t1 * (troj.body[clip_bod[0]].attributes[i].v2 - troj.body[bod].attributes[i].v2);
                    troj.body[clip_bod[0]].attributes[i].v3 = troj.body[bod].attributes[i].v3 + t1 * (troj.body[clip_bod[0]].attributes[i].v3 - troj.body[bod].attributes[i].v3);
                    troj.body[clip_bod[0]].attributes[i].v4 = troj.body[bod].attributes[i].v4 + t1 * (troj.body[clip_bod[0]].attributes[i].v4 - troj.body[bod].attributes[i].v4);
                    troj.body[clip_bod[1]].attributes[i].v1 = troj.body[bod].attributes[i].v1 + t2 * (troj.body[clip_bod[1]].attributes[i].v1 - troj.body[bod].attributes[i].v1);
                    troj.body[clip_bod[1]].attributes[i].v2 = troj.body[bod].attributes[i].v2 + t2 * (troj.body[clip_bod[1]].attributes[i].v2 - troj.body[bod].attributes[i].v2);
                    troj.body[clip_bod[1]].attributes[i].v3 = troj.body[bod].attributes[i].v3 + t2 * (troj.body[clip_bod[1]].attributes[i].v3 - troj.body[bod].attributes[i].v3);
                    troj.body[clip_bod[1]].attributes[i].v4 = troj.body[bod].attributes[i].v4 + t2 * (troj.body[clip_bod[1]].attributes[i].v4 - troj.body[bod].attributes[i].v4);
                }
                trojuholnik.push_back(troj);
                clip_bod.clear();
            }
            else
            {
                trojuholnik.push_back(troj);
                clip_bod.clear();
            }

        }
    }
    
    for (int i = 0; i < trojuholnik.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float w = trojuholnik[i].body[j].gl_Position.w;
            
            trojuholnik[i].body[j].gl_Position.x = ((trojuholnik[i].body[j].gl_Position.x / w) + 1) * (myframe.w / 2);
            trojuholnik[i].body[j].gl_Position.y = ((trojuholnik[i].body[j].gl_Position.y / w) + 1) * (myframe.h / 2);
            trojuholnik[i].body[j].gl_Position.z = trojuholnik[i].body[j].gl_Position.z / w;
        }
    }
    
    for (int i = 0; i < trojuholnik.size(); i++)
    {


        int bod; float h_min;
        trojuholnik[i].body[0].gl_Position.y < trojuholnik[i].body[1].gl_Position.y ? bod = 0 : bod = 1;
        trojuholnik[i].body[bod].gl_Position.y > trojuholnik[i].body[2].gl_Position.y ? bod = 2 : bod = bod;
        h_min = trojuholnik[i].body[bod].gl_Position.y;
        float h_max = std::max(trojuholnik[i].body[0].gl_Position.y, trojuholnik[i].body[1].gl_Position.y);
        h_max = std::max(h_max, trojuholnik[i].body[2].gl_Position.y);
        float center = trojuholnik[i].body[bod].gl_Position.x;

        for (int h = (int)(std::round(h_min)); h < h_max; h++)
        {
            if (h >= 0 && h < myframe.h)
            {
                float w_min = std::min(trojuholnik[i].body[0].gl_Position.x, trojuholnik[i].body[1].gl_Position.x);
                w_min = std::min(w_min, trojuholnik[i].body[2].gl_Position.x);
                float w_max = std::max(trojuholnik[i].body[0].gl_Position.x, trojuholnik[i].body[1].gl_Position.x);
                w_max = std::max(w_max, trojuholnik[i].body[2].gl_Position.x);
                (w_max - w_min) / 2 + w_min;
                for (int w = (int)(std::round(w_min)); w < w_max; w++)
                {
                    if (w >= 0 && w < myframe.w)
                    {
                        float V1, V2, V3, V;

                        if (0 <= (V = (trojuholnik[i].body[2].gl_Position.x - trojuholnik[i].body[0].gl_Position.x) * (trojuholnik[i].body[1].gl_Position.y - trojuholnik[i].body[0].gl_Position.y) - (trojuholnik[i].body[2].gl_Position.y - trojuholnik[i].body[0].gl_Position.y) * (trojuholnik[i].body[1].gl_Position.x - trojuholnik[i].body[0].gl_Position.x)))
                        {
                            if (0 <= (V3 = (w + 0.5f - trojuholnik[i].body[0].gl_Position.x) * (trojuholnik[i].body[1].gl_Position.y - trojuholnik[i].body[0].gl_Position.y) - (h + 0.5f - trojuholnik[i].body[0].gl_Position.y) * (trojuholnik[i].body[1].gl_Position.x - trojuholnik[i].body[0].gl_Position.x)))
                            {
                                if (0 <= (V1 = (w + 0.5f - trojuholnik[i].body[1].gl_Position.x) * (trojuholnik[i].body[2].gl_Position.y - trojuholnik[i].body[1].gl_Position.y) - (h + 0.5f - trojuholnik[i].body[1].gl_Position.y) * (trojuholnik[i].body[2].gl_Position.x - trojuholnik[i].body[1].gl_Position.x)))
                                {
                                    if (0 <= (V2 = (w + 0.5f - trojuholnik[i].body[2].gl_Position.x) * (trojuholnik[i].body[0].gl_Position.y - trojuholnik[i].body[2].gl_Position.y) - (h + 0.5f - trojuholnik[i].body[2].gl_Position.y) * (trojuholnik[i].body[0].gl_Position.x - trojuholnik[i].body[2].gl_Position.x)))
                                    {
                                        int idx = h * myframe.w + w;


                                        f = new InFragment[1];
                                        c = new OutFragment[1];
					c[0].gl_FragColor = glm::vec4(0, 0, 0, 0);

                                        f[0].gl_FragCoord.x = w + 0.5f;
                                        f[0].gl_FragCoord.y = h + 0.5f;
                                        V1 = V1 / V;
                                        V2 = V2 / V;
                                        V3 = V3 / V;
                                        float divisor = V1 / trojuholnik[i].body[0].gl_Position.w
                                            + V2 / trojuholnik[i].body[1].gl_Position.w
                                            + V3 / trojuholnik[i].body[2].gl_Position.w;
                                        V1 = V1 / trojuholnik[i].body[0].gl_Position.w;
                                        V2 = V2 / trojuholnik[i].body[1].gl_Position.w;
                                        V3 = V3 / trojuholnik[i].body[2].gl_Position.w;
                                        f[0].gl_FragCoord.z = (trojuholnik[i].body[0].gl_Position.z * V1 +
                                            trojuholnik[i].body[1].gl_Position.z * V2 +
                                            trojuholnik[i].body[2].gl_Position.z * V3) / divisor;
                                        for(int p = 0; p < program_list[aktiv_prog]->atr_num.size();++p)
                                        {
                                            int num = program_list[aktiv_prog]->atr_num[p];
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::FLOAT)
                                            {
                                                f[0].attributes[num].v1 = (V1 * trojuholnik[i].body[0].attributes[num].v1 + V2 * trojuholnik[i].body[1].attributes[num].v1 + V3 * trojuholnik[i].body[2].attributes[num].v1) / (divisor);;



                                            }
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::VEC2)
                                            {
                                                f[0].attributes[num].v2[0] = (V1 * trojuholnik[i].body[0].attributes[num].v2[0] + V2 * trojuholnik[i].body[1].attributes[num].v2[0] + V3 * trojuholnik[i].body[2].attributes[num].v2[0]) / (divisor);
                                                f[0].attributes[num].v2[1] = (V1 * trojuholnik[i].body[0].attributes[num].v2[1] + V2 * trojuholnik[i].body[1].attributes[num].v2[1] + V3 * trojuholnik[i].body[2].attributes[num].v2[1]) / (divisor);


                                            }
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::VEC3)
                                            {
                                                f[0].attributes[num].v3[0] = (V1 * trojuholnik[i].body[0].attributes[num].v3[0] + V2 * trojuholnik[i].body[1].attributes[num].v3[0] + V3 * trojuholnik[i].body[2].attributes[num].v3[0]) / divisor;
                                                f[0].attributes[num].v3[1] = (V1 * trojuholnik[i].body[0].attributes[num].v3[1] + V2 * trojuholnik[i].body[1].attributes[num].v3[1] + V3 * trojuholnik[i].body[2].attributes[num].v3[1]) / divisor;
                                                f[0].attributes[num].v3[2] = (V1 * trojuholnik[i].body[0].attributes[num].v3[2] + V2 * trojuholnik[i].body[1].attributes[num].v3[2] + V3 * trojuholnik[i].body[2].attributes[num].v3[2]) / divisor;
                                                
                                            }
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::VEC4)
                                            {
                                                f[0].attributes[num].v4[0] = (V1 * trojuholnik[i].body[0].attributes[num].v4[0] + V2 * trojuholnik[i].body[1].attributes[num].v4[0] + V3 * trojuholnik[i].body[2].attributes[num].v4[0]) / divisor;
                                                f[0].attributes[num].v4[1] = (V1 * trojuholnik[i].body[0].attributes[num].v4[1] + V2 * trojuholnik[i].body[1].attributes[num].v4[1] + V3 * trojuholnik[i].body[2].attributes[num].v4[1]) / divisor;
                                                f[0].attributes[num].v4[2] = (V1 * trojuholnik[i].body[0].attributes[num].v4[2] + V2 * trojuholnik[i].body[1].attributes[num].v4[2] + V3 * trojuholnik[i].body[2].attributes[num].v4[2]) / divisor;
                                                f[0].attributes[num].v4[3] = (V1 * trojuholnik[i].body[0].attributes[num].v4[3] + V2 * trojuholnik[i].body[1].attributes[num].v4[3] + V3 * trojuholnik[i].body[2].attributes[num].v4[3]) / divisor;

                                            }
                                        }
                                        program_list[aktiv_prog]->fs(c[0], f[0], program_list[aktiv_prog]->premenne);

                                        if (f[0].gl_FragCoord.z < myframe.hlbka[idx])
                                        {

                                            myframe.hlbka[idx] = f[0].gl_FragCoord.z;
                                            idx *= 4;
                                            for (int i = 0; i < 4; i++)
                                            {
                                                if (c[0].gl_FragColor[i] < 0)
                                                {
                                                    myframe.color[idx + i] = 0;
                                                }
                                                else if (c[0].gl_FragColor[i] > 1)
                                                {
                                                    myframe.color[idx + i] = 255;
                                                }
                                                else
                                                {
                                                    myframe.color[idx + i] = (int)(std::round(c[0].gl_FragColor[i] * 255));
                                                }

                                            }
                                        }
					delete [] f;
					delete [] c;
                                    }
                                }
                            }
                        }
                        else
                        {

                            if (0 <= (V3 = (w + 0.5f - trojuholnik[i].body[1].gl_Position.x) * (trojuholnik[i].body[0].gl_Position.y - trojuholnik[i].body[1].gl_Position.y) - (h + 0.5f - trojuholnik[i].body[1].gl_Position.y) * (trojuholnik[i].body[0].gl_Position.x - trojuholnik[i].body[1].gl_Position.x)))
                            {
                                if (0 <= (V1 = (w + 0.5f - trojuholnik[i].body[2].gl_Position.x) * (trojuholnik[i].body[1].gl_Position.y - trojuholnik[i].body[2].gl_Position.y) - (h + 0.5f - trojuholnik[i].body[2].gl_Position.y) * (trojuholnik[i].body[1].gl_Position.x - trojuholnik[i].body[2].gl_Position.x)))
                                {
                                    if (0 <= (V2 = (w + 0.5f - trojuholnik[i].body[0].gl_Position.x) * (trojuholnik[i].body[2].gl_Position.y - trojuholnik[i].body[0].gl_Position.y) - (h + 0.5f - trojuholnik[i].body[0].gl_Position.y) * (trojuholnik[i].body[2].gl_Position.x - trojuholnik[i].body[0].gl_Position.x)))
                                    {
                                        int idx = h * myframe.w + w;

                                        V = -V;

                                        f = new InFragment[1];
                                        c = new OutFragment[1];
					c[0].gl_FragColor = glm::vec4(0, 0, 0, 0);
                                        f[0].gl_FragCoord.x = w + 0.5f;
                                        f[0].gl_FragCoord.y = h + 0.5f;

                                        V1 = V1 / V;
                                        V2 = V2 / V;
                                        V3 = V3 / V;
                                        float divisor = V1 / trojuholnik[i].body[0].gl_Position.w
                                            + V2 / trojuholnik[i].body[1].gl_Position.w
                                            + V3 / trojuholnik[i].body[2].gl_Position.w;
                                        V1 = V1 / trojuholnik[i].body[0].gl_Position.w;
                                        V2 = V2 / trojuholnik[i].body[1].gl_Position.w;
                                        V3 = V3 / trojuholnik[i].body[2].gl_Position.w;
                                        f[0].gl_FragCoord.z = (trojuholnik[i].body[0].gl_Position.z * V1 +
                                            trojuholnik[i].body[1].gl_Position.z * V2 +
                                            trojuholnik[i].body[2].gl_Position.z * V3) / divisor;
                                        for (int p = 0; p < program_list[aktiv_prog]->atr_num.size(); ++p)
                                        {
                                            int num = program_list[aktiv_prog]->atr_num[p];
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::FLOAT)
                                            {
                                                f[0].attributes[num].v1 = (V1 * trojuholnik[i].body[0].attributes[num].v1 + V2 * trojuholnik[i].body[1].attributes[num].v1 + V3 * trojuholnik[i].body[2].attributes[num].v1) / (divisor);;



                                            }
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::VEC2)
                                            {
                                                f[0].attributes[num].v2[0] = (V1 * trojuholnik[i].body[0].attributes[num].v2[0] + V2 * trojuholnik[i].body[1].attributes[num].v2[0] + V3 * trojuholnik[i].body[2].attributes[num].v2[0]) / (divisor);
                                                f[0].attributes[num].v2[1] = (V1 * trojuholnik[i].body[0].attributes[num].v2[1] + V2 * trojuholnik[i].body[1].attributes[num].v2[1] + V3 * trojuholnik[i].body[2].attributes[num].v2[1]) / (divisor);


                                            }
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::VEC3)
                                            {
                                                f[0].attributes[num].v3[0] = (V1 * trojuholnik[i].body[0].attributes[num].v3[0] + V2 * trojuholnik[i].body[1].attributes[num].v3[0] + V3 * trojuholnik[i].body[2].attributes[num].v3[0]) / divisor;
                                                f[0].attributes[num].v3[1] = (V1 * trojuholnik[i].body[0].attributes[num].v3[1] + V2 * trojuholnik[i].body[1].attributes[num].v3[1] + V3 * trojuholnik[i].body[2].attributes[num].v3[1]) / divisor;
                                                f[0].attributes[num].v3[2] = (V1 * trojuholnik[i].body[0].attributes[num].v3[2] + V2 * trojuholnik[i].body[1].attributes[num].v3[2] + V3 * trojuholnik[i].body[2].attributes[num].v3[2]) / divisor;

                                            }
                                            if (program_list[aktiv_prog]->type[p] == (int)AttributeType::VEC4)
                                            {
                                                f[0].attributes[num].v4[0] = (V1 * trojuholnik[i].body[0].attributes[num].v4[0] + V2 * trojuholnik[i].body[1].attributes[num].v4[0] + V3 * trojuholnik[i].body[2].attributes[num].v4[0]) / divisor;
                                                f[0].attributes[num].v4[1] = (V1 * trojuholnik[i].body[0].attributes[num].v4[1] + V2 * trojuholnik[i].body[1].attributes[num].v4[1] + V3 * trojuholnik[i].body[2].attributes[num].v4[1]) / divisor;
                                                f[0].attributes[num].v4[2] = (V1 * trojuholnik[i].body[0].attributes[num].v4[2] + V2 * trojuholnik[i].body[1].attributes[num].v4[2] + V3 * trojuholnik[i].body[2].attributes[num].v4[2]) / divisor;
                                                f[0].attributes[num].v4[3] = (V1 * trojuholnik[i].body[0].attributes[num].v4[3] + V2 * trojuholnik[i].body[1].attributes[num].v4[3] + V3 * trojuholnik[i].body[2].attributes[num].v4[3]) / divisor;

                                            }
                                        }
                                        program_list[aktiv_prog]->fs(c[0], f[0], program_list[aktiv_prog]->premenne);


                                        if (f[0].gl_FragCoord.z < myframe.hlbka[idx])
                                        {

                                            myframe.hlbka[idx] = f[0].gl_FragCoord.z;
                                            idx *= 4;
                                            for (int i = 0; i < 4; i++)
                                            {
                                                if (c[0].gl_FragColor[i] < 0)
                                                {
                                                    myframe.color[idx + i] = 0;
                                                }
                                                else if (c[0].gl_FragColor[i] > 1)
                                                {
                                                    myframe.color[idx + i] = 255;
                                                }
                                                else
                                                {
                                                    myframe.color[idx + i] = (int)(std::round(c[0].gl_FragColor[i] * 255));
                                                }
                                            }
                                        }
					delete [] f;
					delete [] c;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    

}

/// @}
