#pragma once

class Mesh
{
public:
	Mesh(const XMMATRIX& worldSpace,  Material& mat, std::vector<std::shared_ptr<Intersectable>>& geo)
	{
        
        // Test assmp
        Assimp::Importer imp;
        const aiScene* pModel = imp.ReadFile("Models\\bunnyLow.stl",
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices);

        auto pMesh = pModel->mMeshes[0];

        std::vector<Vertex> vertices;
        vertices.reserve(pMesh->mNumVertices);
        for (UINT i = 0; i < pMesh->mNumVertices; i++)
        {
            vertices.push_back(
                { XMVector4Transform(XMVectorSet(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z, 1.0f), worldSpace),
                XMVector3TransformNormal(XMVectorSet(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z, 0.0f), worldSpace)
                });
        }

        std::vector<UINT> indices;
        indices.reserve(pMesh->mNumFaces * 3);
        for (UINT i = 0; i < pMesh->mNumFaces; i++)
        {
            const auto& face = pMesh->mFaces[i];
            assert(face.mNumIndices == 3);
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
            geo.push_back(std::unique_ptr<Intersectable>(new Triangle(
                { vertices[face.mIndices[0]].position, vertices[face.mIndices[0]].normal },
                { vertices[face.mIndices[1]].position, vertices[face.mIndices[1]].normal },
                { vertices[face.mIndices[2]].position, vertices[face.mIndices[2]].normal },
                mat, false)));
        }
	}
};
