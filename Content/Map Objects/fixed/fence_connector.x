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
 82;
 0.000000;0.006171;0.000000;,
 0.282800;0.006171;0.282800;,
 0.356400;0.006171;-0.181600;,
 0.000000;2.806171;0.000000;,
 0.356400;2.806171;-0.181600;,
 0.282800;2.806171;0.282800;,
 -0.062600;0.006171;-0.395100;,
 -0.062600;2.806171;-0.395100;,
 -0.395100;0.006171;-0.062600;,
 -0.395100;2.806171;-0.062600;,
 -0.181600;0.006171;0.356400;,
 -0.181600;2.806171;0.356400;,
 2.236900;2.168971;0.263900;,
 -2.236900;2.168971;0.263900;,
 -2.236900;2.237571;-0.169400;,
 2.236900;2.237571;-0.169400;,
 -2.236900;1.846671;-0.368600;,
 2.236900;1.846671;-0.368600;,
 -2.236900;1.536471;-0.058400;,
 2.236900;1.536471;-0.058400;,
 -2.236900;1.735571;0.332500;,
 2.236900;1.735571;0.332500;,
 2.236900;0.735571;0.332500;,
 -2.236900;0.735571;0.332500;,
 -2.236900;1.168971;0.263900;,
 2.236900;1.168971;0.263900;,
 2.236900;0.536471;-0.058400;,
 -2.236900;0.536471;-0.058400;,
 2.236900;0.846671;-0.368600;,
 -2.236900;0.846671;-0.368600;,
 2.236900;1.237571;-0.169400;,
 -2.236900;1.237571;-0.169400;,
 0.000000;0.006171;0.000000;,
 0.000000;0.006171;0.000000;,
 0.000000;0.006171;0.000000;,
 0.000000;0.006171;0.000000;,
 0.282800;0.006171;0.282800;,
 0.282800;0.006171;0.282800;,
 0.282800;0.006171;0.282800;,
 0.356400;0.006171;-0.181600;,
 0.356400;0.006171;-0.181600;,
 0.356400;0.006171;-0.181600;,
 0.000000;2.806171;0.000000;,
 0.000000;2.806171;0.000000;,
 0.000000;2.806171;0.000000;,
 0.000000;2.806171;0.000000;,
 0.356400;2.806171;-0.181600;,
 0.356400;2.806171;-0.181600;,
 0.356400;2.806171;-0.181600;,
 0.282800;2.806171;0.282800;,
 0.282800;2.806171;0.282800;,
 0.282800;2.806171;0.282800;,
 -0.062600;0.006171;-0.395100;,
 -0.062600;0.006171;-0.395100;,
 -0.062600;0.006171;-0.395100;,
 -0.062600;2.806171;-0.395100;,
 -0.062600;2.806171;-0.395100;,
 -0.062600;2.806171;-0.395100;,
 -0.395100;0.006171;-0.062600;,
 -0.395100;0.006171;-0.062600;,
 -0.395100;0.006171;-0.062600;,
 -0.395100;2.806171;-0.062600;,
 -0.395100;2.806171;-0.062600;,
 -0.395100;2.806171;-0.062600;,
 -0.181600;0.006171;0.356400;,
 -0.181600;0.006171;0.356400;,
 -0.181600;0.006171;0.356400;,
 -0.181600;2.806171;0.356400;,
 -0.181600;2.806171;0.356400;,
 -0.181600;2.806171;0.356400;,
 2.236900;2.168971;0.263900;,
 -2.236900;2.168971;0.263900;,
 -2.236900;2.237571;-0.169400;,
 2.236900;2.237571;-0.169400;,
 -2.236900;1.846671;-0.368600;,
 2.236900;1.846671;-0.368600;,
 -2.236900;1.536471;-0.058400;,
 2.236900;1.536471;-0.058400;,
 -2.236900;1.735571;0.332500;,
 2.236900;1.735571;0.332500;,
 -2.236900;1.168971;0.263900;,
 2.236900;1.168971;0.263900;;
 40;
 3;35,41,38;,
 3;45,51,48;,
 3;34,54,40;,
 3;44,47,57;,
 3;33,60,53;,
 3;43,56,63;,
 3;32,66,59;,
 3;42,62,69;,
 3;65,0,37;,
 3;3,68,50;,
 3;36,39,46;,
 3;36,46,49;,
 3;2,52,55;,
 3;2,55,4;,
 3;6,58,61;,
 3;6,61,7;,
 3;8,64,67;,
 3;8,67,9;,
 3;5,11,10;,
 3;5,10,1;,
 3;70,73,72;,
 3;70,72,71;,
 3;15,75,74;,
 3;15,74,14;,
 3;17,77,76;,
 3;17,76,16;,
 3;19,79,78;,
 3;19,78,18;,
 3;21,12,13;,
 3;21,13,20;,
 3;22,81,80;,
 3;22,80,23;,
 3;26,22,23;,
 3;26,23,27;,
 3;28,26,27;,
 3;28,27,29;,
 3;30,28,29;,
 3;30,29,31;,
 3;25,30,31;,
 3;25,31,24;;

 MeshNormals {
  82;
  0.000000;-1.000000;0.000000;,
  -0.444729;-0.777452;0.444729;,
  -0.560395;-0.777448;-0.285538;,
  0.000000;1.000000;0.000000;,
  -0.560395;0.777448;-0.285538;,
  -0.444729;0.777452;0.444729;,
  0.098363;-0.777443;-0.621215;,
  0.098363;0.777443;-0.621215;,
  0.621215;-0.777443;-0.098363;,
  0.621215;0.777443;-0.098363;,
  0.285538;-0.777448;0.560395;,
  0.285538;0.777448;0.560395;,
  0.000000;0.707107;0.707107;,
  0.000000;0.707107;0.707107;,
  0.000000;0.891014;-0.453976;,
  0.000000;0.891014;-0.453976;,
  0.000000;-0.156410;-0.987692;,
  0.000000;-0.156410;-0.987692;,
  0.000000;-0.987692;-0.156410;,
  0.000000;-0.987692;-0.156410;,
  0.000000;-0.453976;0.891014;,
  0.000000;-0.453976;0.891014;,
  0.000000;-0.453976;0.891014;,
  0.000000;-0.453976;0.891014;,
  0.000000;0.707107;0.707107;,
  0.000000;0.707107;0.707107;,
  0.000000;-0.987692;-0.156410;,
  0.000000;-0.987692;-0.156410;,
  0.000000;-0.156410;-0.987692;,
  0.000000;-0.156410;-0.987692;,
  0.000000;0.891014;-0.453976;,
  0.000000;0.891014;-0.453976;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  -0.444729;-0.777452;0.444729;,
  -0.444729;-0.777452;0.444729;,
  -0.444729;-0.777452;0.444729;,
  -0.560395;-0.777448;-0.285538;,
  -0.560395;-0.777448;-0.285538;,
  -0.560395;-0.777448;-0.285538;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  0.000000;1.000000;0.000000;,
  -0.560395;0.777448;-0.285538;,
  -0.560395;0.777448;-0.285538;,
  -0.560395;0.777448;-0.285538;,
  -0.444729;0.777452;0.444729;,
  -0.444729;0.777452;0.444729;,
  -0.444729;0.777452;0.444729;,
  0.098363;-0.777443;-0.621215;,
  0.098363;-0.777443;-0.621215;,
  0.098363;-0.777443;-0.621215;,
  0.098363;0.777443;-0.621215;,
  0.098363;0.777443;-0.621215;,
  0.098363;0.777443;-0.621215;,
  0.621215;-0.777443;-0.098363;,
  0.621215;-0.777443;-0.098363;,
  0.621215;-0.777443;-0.098363;,
  0.621215;0.777443;-0.098363;,
  0.621215;0.777443;-0.098363;,
  0.621215;0.777443;-0.098363;,
  0.285538;-0.777448;0.560395;,
  0.285538;-0.777448;0.560395;,
  0.285538;-0.777448;0.560395;,
  0.285538;0.777448;0.560395;,
  0.285538;0.777448;0.560395;,
  0.285538;0.777448;0.560395;,
  0.000000;0.707107;0.707107;,
  0.000000;0.707107;0.707107;,
  0.000000;0.891014;-0.453976;,
  0.000000;0.891014;-0.453976;,
  0.000000;-0.156410;-0.987692;,
  0.000000;-0.156410;-0.987692;,
  0.000000;-0.987692;-0.156410;,
  0.000000;-0.987692;-0.156410;,
  0.000000;-0.453976;0.891014;,
  0.000000;-0.453976;0.891014;,
  0.000000;0.707107;0.707107;,
  0.000000;0.707107;0.707107;;
  40;
  3;35,41,38;,
  3;45,51,48;,
  3;34,54,40;,
  3;44,47,57;,
  3;33,60,53;,
  3;43,56,63;,
  3;32,66,59;,
  3;42,62,69;,
  3;65,0,37;,
  3;3,68,50;,
  3;36,39,46;,
  3;36,46,49;,
  3;2,52,55;,
  3;2,55,4;,
  3;6,58,61;,
  3;6,61,7;,
  3;8,64,67;,
  3;8,67,9;,
  3;5,11,10;,
  3;5,10,1;,
  3;70,73,72;,
  3;70,72,71;,
  3;15,75,74;,
  3;15,74,14;,
  3;17,77,76;,
  3;17,76,16;,
  3;19,79,78;,
  3;19,78,18;,
  3;21,12,13;,
  3;21,13,20;,
  3;22,81,80;,
  3;22,80,23;,
  3;26,22,23;,
  3;26,23,27;,
  3;28,26,27;,
  3;28,27,29;,
  3;30,28,29;,
  3;30,29,31;,
  3;25,30,31;,
  3;25,31,24;;
 }

 MeshTextureCoords {
  82;
  0.900534;0.082309;,
  0.950318;0.989073;,
  0.958399;0.150290;,
  0.900535;0.082309;,
  0.956379;0.359986;,
  0.952338;0.779378;,
  0.956378;0.359986;,
  0.954358;0.569682;,
  0.936349;0.779224;,
  0.938369;0.569528;,
  0.915539;0.988738;,
  0.917559;0.779042;,
  0.380268;0.161724;,
  0.380268;0.971077;,
  0.221538;0.971077;,
  0.221538;0.161724;,
  0.300905;0.971077;,
  0.300905;0.161724;,
  0.380266;0.971077;,
  0.380266;0.161724;,
  0.459651;0.971077;,
  0.459651;0.161724;,
  0.539028;0.161724;,
  0.539028;0.971077;,
  0.856489;0.971077;,
  0.856489;0.161724;,
  0.618395;0.161724;,
  0.618395;0.971077;,
  0.697754;0.161724;,
  0.697754;0.971077;,
  0.777125;0.161724;,
  0.777125;0.971077;,
  0.699635;0.014511;,
  0.424753;0.014511;,
  0.235315;0.099953;,
  0.080446;0.099954;,
  0.968672;0.761834;,
  0.832737;0.014512;,
  0.012648;0.032156;,
  0.984775;0.761989;,
  0.278851;0.014511;,
  0.123982;0.014512;,
  0.729650;0.014511;,
  0.614194;0.014511;,
  0.424754;0.099953;,
  0.167517;0.099954;,
  0.990677;0.149355;,
  0.381218;0.014511;,
  0.123981;0.014512;,
  0.974574;0.149200;,
  0.968332;0.014512;,
  0.235315;0.032156;,
  0.927019;0.149987;,
  0.519473;0.029519;,
  0.330035;0.114961;,
  0.924999;0.359683;,
  0.519474;0.029519;,
  0.330034;0.114961;,
  0.931477;0.359746;,
  0.714642;0.109231;,
  0.439761;0.109231;,
  0.929457;0.569442;,
  0.714643;0.109231;,
  0.599186;0.109231;,
  0.952338;0.779378;,
  0.815092;0.125845;,
  0.614193;0.058047;,
  0.954358;0.569682;,
  0.985976;0.125845;,
  0.815092;0.058047;,
  0.142167;0.161724;,
  0.142167;0.971077;,
  0.062803;0.971077;,
  0.062803;0.161724;,
  0.142168;0.971077;,
  0.142168;0.161724;,
  0.221542;0.971077;,
  0.221542;0.161724;,
  0.300904;0.971077;,
  0.300904;0.161724;,
  0.459644;0.971077;,
  0.459644;0.161724;;
 }

 VertexDuplicationIndices {
  82;
  32;
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
  26,
  27,
  28,
  29,
  30,
  31,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  2,
  2,
  2,
  3,
  3,
  3,
  3,
  4,
  4,
  4,
  5,
  5,
  5,
  6,
  6,
  6,
  7,
  7,
  7,
  8,
  8,
  8,
  9,
  9,
  9,
  10,
  10,
  10,
  11,
  11,
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
  24,
  25;
 }

 MeshMaterialList {
  1;
  40;
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
   0.749020;0.749020;0.749020;1.000000;;
   40.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;

   TextureFilename {
    "C:/Unseen/Projects/Evidyon/media/3d models - static/custom models/WoodenLadder.bmp";
   }
  }
 }
}