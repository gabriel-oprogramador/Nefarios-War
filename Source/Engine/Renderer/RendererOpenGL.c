#ifdef RENDERER_OPENGL
#include <string.h>
#include "GT/Renderer.h"
#include "GT/Platform.h"
#include "GL/ApiGL.h"
#include "GT/Engine.h"

static FShader SDefaultShader = 0;

static FPrimitive InternalCreateQuad();
static FPrimitive InternalCreateCircle();

static cstring SDefaultVertSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aVertPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 vTexCoord;\n"
    "void main(){\n"
    " gl_Position = vec4(aVertPos, 1);\n"
    " vTexCoord = aTexCoord;\n"
    "}";

static cstring SDefaultFragSource =
    "#version 330 core\n"
    "uniform vec4 uColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
    "in vec2 vTexCoord;\n"
    "out vec4 fragColor;\n"
    "vec4 finalColor;\n"
    "void main() {\n"
    " fragColor = uColor;\n"
    "}";

void FRendererInitialize(ERendererApi Renderer) {
  bool bGraphic = false;
  switch(Renderer) {
    case OPENGL_VERSION_3_3: bGraphic = GEngine.graphicApi.OnInitOpenGL(3, 3); break;
    case OPENGL_VERSION_4_5: bGraphic = GEngine.graphicApi.OnInitOpenGL(4, 5); break;
    case OPENGL_VERSION_4_6: bGraphic = GEngine.graphicApi.OnInitOpenGL(4, 6); break;
  }
  if(bGraphic == false){
    GT_LOG(LOG_FATAL, "API-GL: Uninitialized!");
    return;
  }
  SDefaultShader = FShaderCreate(SDefaultVertSource, SDefaultFragSource);
  GT_LOG(LOG_INFO, "API-GL: Created Default Shader Program");
}

void FRendererTerminate() {
}

void FSetClearColor(FColor Color) {
    glClearColor(FCOLOR_GL(Color));
}

void FClearBuffers() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FSetViewport(FRect Viewport) {
  glViewport(Viewport.x, Viewport.y, Viewport.width, Viewport.height);
}

// Shader Program
FShader FShaderCreate(cstring VertexSource, cstring FragmentSource) {
  char logBuffer[BUFFER_LOG_SIZE] = {""};
  uint32 vertexShader, fragmentShader, shaderProgram;
  int32 status = 0;

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const char**)&VertexSource, 0);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(vertexShader, BUFFER_LOG_SIZE, NULL, logBuffer);
    GT_LOG(LOG_ALERT, "Vertex: %s", logBuffer);
    glDeleteShader(vertexShader);
    return SDefaultShader;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (const char**)&FragmentSource, 0);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(fragmentShader, BUFFER_LOG_SIZE, NULL, logBuffer);
    GT_LOG(LOG_ALERT, "Fragment: %s", logBuffer);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return SDefaultShader;
  }

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if(!status) {
    glGetProgramInfoLog(shaderProgram, BUFFER_LOG_SIZE, NULL, logBuffer);
    GT_LOG(LOG_ALERT, "Shader Link: %s", logBuffer);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);
    return SDefaultShader;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  //TODO:Create arenas for Shader
  return shaderProgram;
}

FShader FShaderLoad(cstring VertexPath, cstring FragmentPath) {
  cstring vs = strstr(VertexPath, CONTENT_DIR);
  cstring fs = strstr(FragmentPath, CONTENT_DIR);
  vs = (vs == NULL) ? VertexPath : vs;
  fs = (fs == NULL) ? FragmentPath : fs;

  char* vsBuffer = NULL;
  char* fsBuffer = NULL;

  if(!PReadTextFile(VertexPath, &vsBuffer, NULL, 0)) {
    GT_LOG(LOG_ALERT, "Vertex Shader did not load!");
    return SDefaultShader;
  }

  if(!PReadTextFile(FragmentPath, &fsBuffer, NULL, 0)) {
    GT_LOG(LOG_ALERT, "Fragment Shader did not load!");
    PMemFree(vsBuffer);
    return SDefaultShader;
  }

  FShader shaderProgram = FShaderCreate(vsBuffer, fsBuffer);

  PMemFree(vsBuffer);
  PMemFree(fsBuffer);

  if(shaderProgram == SDefaultShader) {
    GT_LOG(LOG_INFO, "API:GL Use Default Shader Program");
    return SDefaultShader;
  }

  GT_LOG(LOG_INFO, "API:GL Loaded Shader Program => %s | %s", vs, fs);
  return shaderProgram;
}

void FShaderFree(FShader Shader) {
  glDeleteProgram(Shader);
}

FPrimitive FGetPrimitive(EPrimitiveShape Shape) {
  FPrimitive retVal = {0};
  switch(Shape) {
    case PS_QUAD: retVal = InternalCreateQuad(); break;
    case PS_CIRCLE: retVal = InternalCreateCircle(); break;
  }

  return retVal;
}

void FDrawPrimitive(FPrimitive Primitive, FColor Color) {
  glUseProgram(SDefaultShader);
  glUniform4fv(Primitive.uColorID, 1, (const float*)&Color);
  glBindVertexArray(Primitive.objectID);
  glDrawElements(GL_TRIANGLES, Primitive.indices, GL_UNSIGNED_INT, NULL);
  glBindVertexArray(0);
  glUseProgram(0);
}

// Intenal Functions
static FPrimitive InternalCreateQuad() {
  static bool bCreated = false;
  static FPrimitive primitive = {0};

  if(bCreated) {
    return primitive;
  }

  uint32 VAO, VBO, EBO;

  // clang-format off
  float vertices[] = {
    /*  Position     */  /*Tex Coord*/
       1.f,  1.f,  0.f,    1.f, 1.f,   // Top Right
       1.f, -1.f,  0.f,    1.f, 0.f,   // Bottom Right
      -1.f, -1.f,  0.f,    0.f, 0.f,   // Bottom Left
      -1.f,  1.f,  0.f,    0.f, 1.f,   // Top Left
  };

  int32 indices[] = {0, 1, 2, 2, 3, 0};
  // clang-format on

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  bCreated = true;
  primitive.objectID = VAO;
  primitive.indices = 6;
  primitive.shape = PS_QUAD;
  primitive.uColorID = glGetUniformLocation(SDefaultShader, "uColor");

  return primitive;
}

static FPrimitive InternalCreateCircle() {
  static bool bCreated = false;
  static FPrimitive primitive = {0};

  if(bCreated) {
    return primitive;
  }

  uint32 VAO, VBO, EBO, bCircle;
  const uint32 SEGMENTS = 100;
  const float RADIUS = 0.5;

  // clang-format off
  float vertices[(SEGMENTS + 1) * 2];
  float texCoord[(SEGMENTS + 1) * 2];
  int32 indices[SEGMENTS * 3];

  vertices[0] = 0.f;
  vertices[1] = 0.f;
  texCoord[0] = 0.5f;
  texCoord[1] = 0.5f;

  for(int32 i = 0; i <= SEGMENTS; i ++){
    float theta = 2.0f * M_PI * (float)i / (float)SEGMENTS;
    vertices[2 * (i + 1)] = RADIUS * cosf(theta);     // X
    vertices[2 * (i + 1) + 1] = RADIUS * sinf(theta); // Y

    texCoord[2 * (i + 1)] = 0.5f + 0.5f * cosf(theta);
    texCoord[2 * (i + 1) + 1] = 0.5f + 0.5f * sinf(theta);

    if(i < SEGMENTS){
      indices[i * 3] = 0;
      indices[i * 3 + 1] = i  + 1;
      indices[i * 3 + 2] = (i + 1) % SEGMENTS + 1;
    }
  }
  // clang-format on

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &bCircle);
  glBindBuffer(GL_ARRAY_BUFFER, bCircle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  bCreated = true;
  primitive.objectID = VAO;
  primitive.indices = SEGMENTS * 3;
  primitive.shape = PS_CIRCLE;
  primitive.uColorID = glGetUniformLocation(SDefaultShader, "uColor");

  return primitive;
}

#endif  // USE_OPENGL
