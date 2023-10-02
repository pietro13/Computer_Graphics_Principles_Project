/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <vector>


/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    std::vector<void*> buffer_list;
    std::vector<BufferID> buf_id;
    struct frame
    {
        std::vector<float> hlbka;
        std::vector<uint8_t> color;
        int h;
        int w;
        
    };
    frame myframe;

    struct hlava
    {
        AttributeType type;
        uint64_t stride;
        uint64_t offset;
        BufferID buffer;
        bool enable = false;
    };

    struct _index
    {
        IndexType type;
        BufferID buffer;
    };

    struct tabulka
    {
        hlava hlavy[maxAttributes];
        _index index;
        bool ind = false;
    };
    std::vector<tabulka*> vertex_list;
    std::vector<ObjectID> ver_id;
    VertexPullerID aktiv_vertex;

    struct program
    {
        VertexShader vs;
        FragmentShader fs;
        Uniforms premenne;
        std::vector<int> type;
        std::vector<int>  atr_num;
        

    };
    std::vector<int> clip_bod;
    std::vector<int> non_clip_bod;
    uint8_t poradie1;
    uint16_t poradie2;
    uint32_t poradie3;
    int ten_type;
    std::vector<program*> program_list;
    std::vector<ProgramID> pro_id;
    ProgramID aktiv_prog;

    struct trojuhol
    {
        OutVertex body[3];
    };
    InFragment* f;
    OutFragment* c;
    InVertex* akt_ver;
    /// @}
};


