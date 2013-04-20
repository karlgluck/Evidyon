xof 0303txt 0032
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}

template AnimTicksPerSecond {
 <9e415a43-7ba6-4a73-8743-b73d47e88476>
 DWORD AnimTicksPerSecond;
}


AnimTicksPerSecond {
 24;
}

Mesh {
 27;
 1.574500;0.848500;-0.967500;,
 1.574500;1.663500;-0.996800;,
 0.000000;1.663500;-0.996800;,
 0.000000;0.848500;-0.967500;,
 1.574500;0.001500;-0.896200;,
 0.000000;0.001500;-0.896200;,
 -1.574500;1.663500;-0.996800;,
 -1.574500;0.848500;-0.967500;,
 -1.574500;0.001500;-0.896200;,
 -0.787200;3.344600;-0.000500;,
 0.000000;1.648300;0.997300;,
 -1.574500;1.648300;0.997300;,
 -1.574500;1.788200;-0.000500;,
 0.000000;1.788200;-0.000500;,
 0.787200;3.344600;-0.000500;,
 1.574500;1.648300;0.997300;,
 1.574500;1.788200;-0.000500;,
 -1.574500;0.848500;0.967500;,
 -1.574500;1.663500;0.996800;,
 -0.000000;1.663500;0.996800;,
 -0.000000;0.848500;0.967500;,
 -1.574500;0.001500;0.896200;,
 -0.000000;0.001500;0.896200;,
 1.574500;1.663500;0.996800;,
 1.574500;0.848500;0.967500;,
 1.574500;0.001500;0.896200;,
 0.000000;1.663500;-0.996800;;
 28;
 3;0,3,26;,
 3;0,2,1;,
 3;4,5,3;,
 3;4,3,0;,
 3;3,7,6;,
 3;3,6,2;,
 3;5,8,7;,
 3;5,7,3;,
 3;9,11,10;,
 3;12,11,9;,
 3;9,10,13;,
 3;9,2,6;,
 3;13,2,9;,
 3;9,6,12;,
 3;14,10,15;,
 3;13,10,14;,
 3;14,15,16;,
 3;14,1,2;,
 3;16,1,14;,
 3;14,2,13;,
 3;17,20,19;,
 3;17,19,18;,
 3;21,22,20;,
 3;21,20,17;,
 3;20,24,23;,
 3;20,23,19;,
 3;22,25,24;,
 3;22,24,20;;

 MeshNormals {
  27;
  0.000000;-0.059908;-0.998204;,
  -0.386250;0.400869;-0.830731;,
  0.000000;0.434596;-0.900626;,
  0.000000;-0.059908;-0.998204;,
  0.000000;-0.083896;-0.996475;,
  0.000000;-0.083896;-0.996475;,
  0.386250;0.400869;-0.830731;,
  0.000000;-0.059908;-0.998204;,
  0.000000;-0.083896;-0.996475;,
  0.000000;0.999986;0.005371;,
  0.000000;0.719179;0.694825;,
  0.555969;0.597780;0.577545;,
  0.892325;0.451382;0.003357;,
  0.000000;0.999972;0.007477;,
  0.000000;0.999986;0.005371;,
  -0.555969;0.597780;0.577545;,
  -0.892325;0.451382;0.003357;,
  -0.000000;-0.059908;0.998204;,
  0.386250;0.400869;0.830731;,
  -0.000000;0.434596;0.900626;,
  -0.000000;-0.059908;0.998204;,
  -0.000000;-0.083896;0.996475;,
  -0.000000;-0.083896;0.996475;,
  -0.386250;0.400869;0.830731;,
  -0.000000;-0.059908;0.998204;,
  -0.000000;-0.083896;0.996475;,
  0.000000;0.434596;-0.900626;;
  28;
  3;0,3,26;,
  3;0,2,1;,
  3;4,5,3;,
  3;4,3,0;,
  3;3,7,6;,
  3;3,6,2;,
  3;5,8,7;,
  3;5,7,3;,
  3;9,11,10;,
  3;12,11,9;,
  3;9,10,13;,
  3;9,2,6;,
  3;13,2,9;,
  3;9,6,12;,
  3;14,10,15;,
  3;13,10,14;,
  3;14,15,16;,
  3;14,1,2;,
  3;16,1,14;,
  3;14,2,13;,
  3;17,20,19;,
  3;17,19,18;,
  3;21,22,20;,
  3;21,20,17;,
  3;20,24,23;,
  3;20,23,19;,
  3;22,25,24;,
  3;22,24,20;;
 }

 MeshTextureCoords {
  27;
  0.226491;0.025068;,
  0.401402;0.025068;,
  0.398055;0.317528;,
  0.226491;0.317528;,
  0.068606;0.025068;,
  0.068606;0.317528;,
  0.395544;0.609988;,
  0.226491;0.609988;,
  0.068606;0.609988;,
  0.651848;0.463749;,
  0.894503;0.317528;,
  0.894503;0.609988;,
  0.653774;0.609988;,
  0.653774;0.317528;,
  0.651848;0.171308;,
  0.894503;0.025068;,
  0.653774;0.025068;,
  0.134033;0.803357;,
  0.134314;0.622650;,
  0.483197;0.623193;,
  0.482916;0.803900;,
  0.133740;0.991702;,
  0.482623;0.992245;,
  0.832080;0.623736;,
  0.831798;0.804443;,
  0.831505;0.992788;,
  0.401402;0.317528;;
 }

 VertexDuplicationIndices {
  27;
  26;
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  2;
 }

 MeshMaterialList {
  1;
  28;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;

  Material Texture {
   0.752941;0.752941;0.752941;1.000000;;
   40.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "C:/Unseen/Projects/Evidyon/media/3d models - static/custom models/stone.png";
   }
  }
 }
}