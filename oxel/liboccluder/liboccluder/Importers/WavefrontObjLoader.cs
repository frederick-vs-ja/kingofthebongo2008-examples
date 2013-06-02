﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

// WavefrontObjLoader.cs
//
// Wavefront .OBJ 3d fileformat loader in C# (csharp dot net)
//
// Copyright (C) 2012 David Jeske, and given to the public domain
//
// Originally Copied from DXGfx code by Guillaume Randon, Copyright (C) 2005, BSD License (See below notice)
//
// BSD License  
// DXGfx® - http://www.eteractions.com
// Copyright (c) 2005
// by Guillaume Randon
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
// AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

namespace Util3d {

    public class WavefrontObjLoader {
        // file format info...
        //
        // http://en.wikipedia.org/wiki/Wavefront_.obj_file
        // http://www.fileformat.info/format/wavefrontobj/egff.htm
        // http://www.fileformat.info/format/material/
        //
        // NOTE: OBJ uses CIE-XYZ color space...
        //
        // http://www.codeproject.com/Articles/19045/Manipulating-colors-in-NET-Part-1
        // 
        // TODO: handle multi-line faces continued with "\"
        // TODO: handle negative vertex indices in face specification
        // TODO: handle "s" smoothing group
        // TODO: handle "Tr"/"d" material transparency/alpha
        //
        // NOTE: OBJ puts (0,0) in the Upper Left, OpenGL Lower Left, DirectX Lower Left
        // 
        // http://stackoverflow.com/questions/4233152/how-to-setup-calculate-texturebuffer-in-gltexcoordpointer-when-importing-from-ob

        public struct Vector2_UV {
            public float U, V;
            public Vector2_UV(float u, float v) {
                this.U = u; this.V = v;
            }
            
        }

        public struct Vector2 {
            public float X, Y;
            public Vector2(float x, float y) {
                this.X = x; this.Y = y;
            }
        }
        public struct Vector3 {
            public float X, Y, Z;
            public Vector3(float x, float y, float z) {
                this.X = x; this.Y = y; this.Z = z;
            }
        }

        public struct Face {
            public Int16[] v_idx;
            public Int16[] n_idx;
            public Int16[] tex_idx;
        }
        
        public int numFaces = 0;        
        public int numIndices = 0;
        public bool hasNormals = false;        
        
        // these are all indexed by "raw" vertex number from the OBJ file
        // NOTE: these indicies are shared by the Faces in each material, so
        //       if you need per material indicies, you'll need to rebuild your own
        //       vertex lists and indicies.

        public List<Vector2_UV> texCoords = new List<Vector2_UV>();
        public List<Vector3> normals = new List<Vector3>();
        public List<Vector3> positions = new List<Vector3>();

        public List<MaterialFromObj> materials = new List<MaterialFromObj>();

        public enum WffObjIlluminationMode {
            ColorOnAmbientOff = 0,
            ColorOnAmbiendOn = 1,
            HighlightOn = 2,
            ReflectionOnRayTraceOn = 3,
            TransparentyGlassOn_ReflectionRayTraceOn = 4,
            ReflectionFresnelAndRayTraceOn = 5,
            TransparencyRefractionOn_ReflectionFresnelOffRayTraceOn = 6,
            TransparentyRefractionOn_ReflectionFresnelOnRayTraceOn = 7,
            ReflectionOn_RayTraceOff = 8,
            TransparencyGlassOn_ReflectionRayTraceOff = 9,
            CastsShadowsOntoInvisibleSurfaces = 10
        }

        /// <summary>
        /// This structure is used to store material information.
        /// </summary>
        public class MaterialFromObj {
            public string name;

            public bool hasAmbient;
            public Vector3 vAmbient;       // Ka

            public bool hasDiffuse;
            public Vector3 vDiffuse;       // Kd
            
            public bool hasSpecular;            
            public Vector3 vSpecular;      // Ks
            public float vSpecularWeight;  // Ns

            // textures
            public string ambientTextureResourceName;    // map_Ka
            public string diffuseTextureResourceName;    // map_Kd
            public string specularTextureResourceName;   // map_Ks
            public string bumpTextureResourceName;       // map_bump || bump
            public string alphaMaskTextureResourceName;  // map_d

            public bool hasIlluminationMode;
            public WffObjIlluminationMode illuminationMode;  // illum

            public bool hasTransparency;
            public float fTransparency;

            public int nbrIndices;
            public List<Face> faces = new List<Face>();
        }


        /// <summary>
        /// This method is used to split string in a list of strings based on the separator passed to hte method.
        /// </summary>
        /// <param name="strIn"></param>
        /// <param name="separator"></param>
        /// <returns></returns>
        private string[] FilteredSplit(string strIn, char[] separator) {
            string[] valuesUnfiltered = strIn.Split(separator);

            // Sometime if we have a white space at the beginning of the string, split
            // will remove an empty string. Let's remove that.
            List<string> listOfValues = new List<string>();
            foreach (string str in valuesUnfiltered) {
                if (str != "") {
                    listOfValues.Add(str);
                }
            }
            string[] values = listOfValues.ToArray();

            return values;
        }

        private void ASSERT(bool test_true, string reason) {
            if (!test_true) {
                throw new Exception("WavefrontObjLoader Error: " + reason);
            }
        }

        private Vector3 readVector3(string strIn, char[] separator) {
            string[] values = FilteredSplit(strIn, separator);

            ASSERT(values.Length == 3, "readVector3 found wrong number of vectors : " + strIn);
            return new Vector3(
                float.Parse(values[0]),
                float.Parse(values[1]),
                float.Parse(values[2]));
        }

        private Vector2 readVector2(string strIn, char[] separator) {
            string[] values = FilteredSplit(strIn, separator);

            ASSERT(values.Length == 2, "readVector2 found wrong number of vectors : " + strIn);
            return new Vector2(
                float.Parse(values[0]),
                float.Parse(values[1]));

        }

        private Vector2_UV readVector2_UV(string strIn, char[] separator) {
            string[] values = FilteredSplit(strIn, separator);

            ASSERT(values.Length == 2, "readVector2_UV found wrong number of vectors : " + strIn);
            return new Vector2_UV(
                float.Parse(values[0]),
                float.Parse(values[1]));
        }


        private MaterialFromObj createImplicitMaterial() {
            MaterialFromObj makeMaterial = new MaterialFromObj();
            materials.Add(makeMaterial);

            makeMaterial.name = "[ Implicit Material ]";

            return makeMaterial;            
        }
            

        /// <summary>
        /// This method is used to load information stored in .mtl files referenced by the .obj file.
        /// </summary>
        /// <param name="d3ddevice"></param>
        /// <param name="file"></param>
       
        public void parseMTL(string file) {
            MaterialFromObj parseMaterial = new MaterialFromObj();
            
            bool first = true;

            // ask the delegate to open the material file for us..
            StreamReader sr = new StreamReader(this.opendelegate(file));            

            //Read the first line of text
            string line = sr.ReadLine();

            //Continue to read until you reach end of file
            while (line != null) {
                string[] tokens = line.Split(" ".ToArray(), 2);
                if (tokens.Length < 2) {
                    goto next_line;
                }

                string firstToken = tokens[0];
                string lineContent = tokens[1];


                switch (firstToken) {
                    case "#":
                        // Nothing to read, these are comments.
                        break;
                    case "newmtl":  // create new named material                
                        if (!first) {
                            materials.Add(parseMaterial);
                            parseMaterial = new MaterialFromObj();
                        }
                        first = false;
                        parseMaterial.name = lineContent;
                        break;
                    case "Ka": // ambient color
                        parseMaterial.vAmbient = readVector3(lineContent, null);
                        parseMaterial.hasAmbient = true;
                        break;
                    case "Kd": // diffuse color
                        parseMaterial.vDiffuse = readVector3(lineContent, null);
                        parseMaterial.hasDiffuse = true;
                        break;
                    case "Ks": // specular color (weighted by Ns)                                 
                        parseMaterial.vSpecular = readVector3(lineContent,null);
                        parseMaterial.hasSpecular = true;
                        break;
                    case "Ns": // specular color weight                
                        parseMaterial.vSpecularWeight = float.Parse(lineContent);   
                        break;
                    case "d":
                    case "Tr": // transparency / dissolve (i.e. alpha)
                        parseMaterial.fTransparency = float.Parse(lineContent);
                        parseMaterial.hasTransparency = true;
                        break;
                    case "illum": // illumination mode                           
                        parseMaterial.hasIlluminationMode = true;
                        parseMaterial.illuminationMode = (WffObjIlluminationMode) int.Parse(lineContent);
                        break;
                    case "map_Kd": // diffuse color map                
                        parseMaterial.diffuseTextureResourceName = lineContent;
                        break;
                    case "map_Ka": // ambient color map
                        parseMaterial.ambientTextureResourceName = lineContent;
                        break;
                    case "map_Ks": // specular color map                
                        parseMaterial.specularTextureResourceName = lineContent;
                        break;
                    case "map_d": // alpha mask map                
                        parseMaterial.alphaMaskTextureResourceName = lineContent;
                        parseMaterial.hasTransparency = true;
                        break;
                    case "bump":
                    case "map_bump": // bump map                
                        parseMaterial.bumpTextureResourceName = lineContent;
                        break;
                }

            next_line:
                //Read the next line
                line = sr.ReadLine();
            }
            materials.Add(parseMaterial);

            //close the file
            sr.Close();
        }

        private void parseOBJ(StreamReader sr) {
            MaterialFromObj currentMaterial = null;

            //Read the first line of text
            string line = sr.ReadLine();

            //Continue to read until you reach end of file            
            while (line != null) 
            {
                string[] tokens = line.Split(" ".ToArray(), 2);
                if (tokens.Length < 2) {
                    goto next_line;
                }

                string firstToken = tokens[0];
                string lineContent = tokens[1];

                switch(firstToken) {
                    case "#":   // Nothing to read, these are comments.                        
                        break;
                    case "v":   // Vertex position
                        positions.Add(readVector3(lineContent, null));
                        break;
                    case "vn":  // vertex normal direction vector
                        normals.Add(readVector3(lineContent, null));
                        break;
                    case "vt":  // Vertex texcoordinate
                        texCoords.Add(readVector2_UV(lineContent,null));
                        break;
                    case "f":   // Face                    
                        string[] values = FilteredSplit(lineContent, null);
                        int numPoints = values.Length;
                    
                        Face face = new Face(); 
                        face.v_idx = new Int16[numPoints];
                        face.n_idx = new Int16[numPoints];
                        face.tex_idx = new Int16[numPoints];  // todo: how do outside clients know if there were texcoords or not?!?! 

                        for (int i = 0; i < numPoints; i++)
                        {
                            string[] indexes = values[i].Split('/');    //  "loc_index[/tex_index[/normal_index]]"

                            int iPosition = (int.Parse(indexes[0]) - 1);  // adjust 1-based index                    
                            if (iPosition < 0) { iPosition += positions.Count + 1; } // adjust negative indicies
                            face.v_idx[i] = (Int16)iPosition; 
                        
                            numIndices++;                        

                            if (indexes.Length > 1)
                            {
                                int iTexCoord = int.Parse(indexes[1]) - 1; // adjust 1-based index
                                if (iTexCoord < 0) { iTexCoord += texCoords.Count + 1; }  // adjust negative indicies

                                face.tex_idx[i] = (Int16)iTexCoord;

                                if (indexes.Length > 2)
                                {
                                    hasNormals = true;
                                    int iNormal = int.Parse(indexes[2]) - 1; // adjust 1 based index
                                    if (iNormal < 0) { iNormal += normals.Count + 1; } // adjust negative indicies

                                    face.n_idx[i] = (Int16)iNormal;                                
                                }
                            }
                        }
                        if (currentMaterial == null) {
                            // no material in file, so create one
                            currentMaterial = createImplicitMaterial();
                        }
                        currentMaterial.faces.Add(face);
                        currentMaterial.nbrIndices += face.v_idx.Length;
                        numFaces++;                                            
                        break;
                    case "mtllib":  // load named material file
                        string mtlFile = lineContent;
                        parseMTL(mtlFile);
                        break;
                    case "usemtl":  // use named material (from material file previously loaded)
                        bool found = false;

                        string matName = lineContent;

                        for (int i = 0; i < materials.Count; i++)
                        {
                            if (matName.Equals(materials[i].name))
                            {
                                found = true;
                                currentMaterial = materials[i];                            
                            }
                        }

                        if (!found)
                        {
                            throw new Exception("Materials are already loaded so we should have it!");
                        }
                        break;
                }                

            next_line:
                //Read the next line
                line = sr.ReadLine();
            }

            //close the file
            sr.Close();
        }

        public delegate Stream openNamedFile(string resource_name); 
        openNamedFile opendelegate;

        public WavefrontObjLoader(string obj_file_name, openNamedFile opendelegate) {
            this.opendelegate = opendelegate;
            Stream fs = opendelegate(obj_file_name);            
            this.parseOBJ(new StreamReader(fs));
        }
    }
}
