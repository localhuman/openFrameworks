#include "ofModelUtils.h"
#include "aiConfig.h"
#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"
#include "ofColor.h"

//--------------------------------------------------------------
static inline ofColor aiColorToOfColor(const aiColor4D& c){
	return ofColor(255*c.r,255*c.g,255*c.b,255*c.a);
}

//--------------------------------------------------------------
void aiMeshToOfVertexData(const aiMesh* aim, ofVertexData& ofm);
void aiMeshToOfVertexData(const aiMesh* aim, ofVertexData& ofm){
	// default to triangle mode
	ofm.setMode(OF_TRIANGLES_MODE);
	
	// copy vertices
	for (int i=0; i < (int)aim->mNumVertices;i++){
		ofm.addVertex(ofVec3f(aim->mVertices[i].x,aim->mVertices[i].y,aim->mVertices[i].z));
	}

	if(aim->HasNormals()){
		for (int i=0; i < (int)aim->mNumVertices;i++){
			ofm.addNormal(ofVec3f(aim->mNormals[i].x,aim->mNormals[i].y,aim->mNormals[i].z));
		}
	}
	
	// aiVector3D * 	mTextureCoords [AI_MAX_NUMBER_OF_TEXTURECOORDS]
	// just one for now
	if(aim->GetNumUVChannels()>0){
		for (int i=0; i < (int)aim->mNumVertices;i++){
			ofm.addTexCoord(ofVec2f(aim->mTextureCoords[0][i].x,aim->mTextureCoords[0][i].y));
		}
	}
	
	//aiColor4D * 	mColors [AI_MAX_NUMBER_OF_COLOR_SETS]	
	// just one for now		
	if(aim->GetNumColorChannels()>0){
		for (int i=0; i < (int)aim->mNumVertices;i++){
			ofm.addColor(aiColorToOfColor(aim->mColors[0][i]));
		}
	}
	
	for (int i=0; i <(int) aim->mNumFaces;i++){
		if(aim->mFaces[i].mNumIndices>3){
			ofLog(OF_LOG_WARNING,"non-triangular face found: model face # " + ofToString(i));
		}
		for (int j=0; j<(int)aim->mFaces[i].mNumIndices; j++){
			ofm.addIndex(aim->mFaces[i].mIndices[j]);
		}
	}	
	
	ofm.setName(string(aim->mName.data));
	//	ofm.materialId = aim->mMaterialIndex;	
}

//--------------------------------------------------------------
void loadMeshes(string modelName, vector<ofMesh>& m);
void loadMeshes(string modelName,vector<ofMesh>& m){
    string filepath = ofToDataPath(modelName);
	
    ofLog(OF_LOG_VERBOSE, "loading meshes from %s", filepath.c_str());
    
	//TODO: import modes
    // only ever give us triangles.
    aiSetImportPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT );
    aiSetImportPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, true);
    
    // aiProcess_FlipUVs is for VAR code. Not needed otherwise. Not sure why.
    aiScene * scene = (aiScene*) aiImportFile(filepath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_OptimizeGraph | aiProcess_Triangulate | aiProcess_FlipUVs | 0 );	
    if(scene != NULL){        
        ofLog(OF_LOG_VERBOSE, "initted scene with %i meshes", scene->mNumMeshes);
		m.resize(scene->mNumMeshes);
		for (unsigned int i = 0; i < scene->mNumMeshes; i++){
			ofLog(OF_LOG_VERBOSE, "loading mesh %u", i);
			// current mesh we are introspecting
			aiMesh* aMesh = scene->mMeshes[i];
			m[i] = ofMesh();
			if(m[i].vertexData == NULL){
				m[i].vertexData = new ofVertexData();
			}
			aiMeshToOfVertexData(aMesh,*m[i].vertexData);
			m[i].setUseIndices();
		}
    }
}	

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofLoadModel(string modelName, ofModel & model){

	loadMeshes(modelName, model.meshes);

}
