#include "Racket.h"
#include <VectorToMatrix.h>

namespace
{
void affectRacketVelocity_(LONG x, BallPhysics::ShapelessObject& obj) noexcept
{
	obj.velocity = Vector(float(x) * 50., 0);
}
}

void Racket::initialize(UI& ui,
						BallGraphics::Camera& camera,
						BallInput::Input & input,
						BallPhysics::World & world,
						Vector gameSize) noexcept
{
	ui_ = &ui;
	camera_ = &camera;
	input_ = &input;
	world_ = &world;
	gameSize_ = gameSize;

	{
		texture_ = ui_->getGraphics().createTexture("../../BallsGame/Resources/brick1.tga");

		BallGraphics::ObjFile objFile;
		bool loaded = objFile.load("../../BallsGame/Resources/Racket.3dobj");
		assert(loaded);

		std::vector<Vector> originalLine;
		std::vector<bool> addedVertices(objFile.vertices.size(), false);
		auto addThis = [this, &objFile, &addedVertices, &originalLine](int v, int n)
		{
			if (!addedVertices[v - 1])
			{
				auto& vert = objFile.vertices[v - 1];
				if (vert.z == 0.f && dot_product(Vector{ objFile.normals[n - 1].x, objFile.normals[n - 1].y }, Vector{ 0.f, 1.f }) > 0.f)
				{
					originalLine.push_back(Vector{ vert.x, vert.y });
					addedVertices[v - 1] = true;
				}
			}
		};
		for (const auto& face : objFile.faces)
		{
			addThis(face.vIndex1, face.nIndex1);
			addThis(face.vIndex2, face.nIndex2);
			addThis(face.vIndex3, face.nIndex3);
		}
		std::sort(originalLine.begin(), originalLine.end(), [](const Vector& arg1, const Vector& arg2)
		{
			return arg1.x < arg2.x;
		});

		for (size_t i = 0; i < levelNumber_; i++)
		{
			initLevel_(i, [i](const Vector& v)
			{
				return Vector(v.x * (i / 10. + 1), v.y) * 50;
			}, 50.f, originalLine, objFile);
		}
	}
}

void Racket::enter() noexcept
{
	boundaryGroup_ = world_->addCollisionGroup();
	racketGroup_ = world_->addCollisionGroup();

	currentLevel_ = defaultLevel_;

	const Vector racketPos{ 0, -gameSize_.y / 2.f };
	auto racketId = world_->addLine(BallPhysics::ShapelessObject{ racketPos, { 0., 0. }, std::numeric_limits<float>::infinity() },
									lines_[currentLevel_].line);
	racketIds_.push_back(racketId);
	boundaryId_ = world_->addCircle(BallPhysics::ShapelessObject{ racketPos, { 0., 0. }, 1.f }, { lines_[currentLevel_].halfWidth });
	world_->addToCollisionGroup(boundaryGroup_, boundaryId_);

	addMeshInst_(racketId);
	world_->addToCollisionGroup(racketGroup_, racketIds_.back());
}

void Racket::leave() noexcept
{
	racketIds_.clear();
	input_->setOnMouseMove([](LONG, LONG) {});
	removeMeshInst_();
}

void Racket::increaseWidth() noexcept
{
	currentLevel_++;

	auto pos = world_->getPosition(racketIds_.back());
	if (abs(pos.x) > gameSize_.x / 2 - lines_[currentLevel_].halfWidth)
	{
		pos.x = (gameSize_.x / 2 - lines_[currentLevel_].halfWidth) * ((pos.x > 0) * 2 - 1) / 1.000001;
	}

	auto racketId = world_->addLine(BallPhysics::ShapelessObject{ pos,
									world_->getVelocity(racketIds_.back()),
									std::numeric_limits<float>::infinity() },
									lines_[currentLevel_].line);
	racketIds_.push_back(racketId);
	world_->addToCollisionGroup(racketGroup_, racketId);

	world_->removeBody(boundaryId_);
	boundaryId_ = world_->addCircle(BallPhysics::ShapelessObject{ pos,
									world_->getVelocity(racketIds_.back()), 1.f }, { lines_[currentLevel_].halfWidth });
	world_->addToCollisionGroup(boundaryGroup_, boundaryId_);

	removeMeshInst_();
	addMeshInst_(racketId);
}

void Racket::decreaseWidth() noexcept
{
	if (racketIds_.size() > 1)
	{
		assert(currentLevel_ > 1);
		currentLevel_--;
		world_->removeBody(racketIds_.back());
		racketIds_.pop_back();
		auto racketId = racketIds_.back();

		world_->removeBody(boundaryId_);
		boundaryId_ = world_->addCircle(BallPhysics::ShapelessObject{ world_->getPosition(racketIds_.back()),
										world_->getVelocity(racketIds_.back()), 1.f }, { lines_[currentLevel_].halfWidth });
		world_->addToCollisionGroup(boundaryGroup_, boundaryId_);

		removeMeshInst_();
		addMeshInst_(racketId);
	}
}

BallPhysics::Id Racket::getGroup() const noexcept
{
	return racketGroup_;
}

BallPhysics::Id Racket::getBoundaryGroup() const noexcept
{
	return boundaryGroup_;
}

void Racket::stopMotion() noexcept
{
	affect_([](BallPhysics::ShapelessObject& obj)
	{
		affectRacketVelocity_(0, obj);
	});
	affect_([this](BallPhysics::ShapelessObject& obj)
	{
		if (abs(obj.position.x) > (gameSize_.x / 2 - lines_[currentLevel_].halfWidth) / 1.000001)
			obj.position.x = (gameSize_.x / 2 - lines_[currentLevel_].halfWidth) * ((obj.position.x > 0) * 2 - 1) / 1.000001;
	});
}

void Racket::processMouse(LONG x, LONG y) noexcept
{
	affect_([x](BallPhysics::ShapelessObject& obj)
	{
		affectRacketVelocity_(x, obj);
	});
}

void Racket::initLevel_(size_t level, const std::function<Vector(const Vector&)>& func, float zMultiplier,
						const std::vector<Vector>& original, const BallGraphics::ObjFile& objFile) noexcept
{
	lines_[level].line.reserve(original.size());
	for (const Vector& v : original)
	{
		lines_[level].line.push_back(func(v));
	}
	auto maxYIt = std::max_element(lines_[level].line.begin(), lines_[level].line.end(), [](const Vector& v1, const Vector& v2)
	{
		return v1.x < v2.x;
	});
	lines_[level].halfWidth = maxYIt->x;

	auto mesh = ui_->getEffectTexture().createMesh(texture_);
	bool loaded = mesh->load(objFile, [&func, zMultiplier](float x, float y, float z)
	{
		Vector v{ func(Vector{ x, y }) };
		return Position{v.x, v.y, zMultiplier * z};
	});
	assert(loaded);
	meshes_[level] = ui_->getGraphics().addMesh(std::move(mesh));
}

void Racket::affect_(BallPhysics::Body::VelocityChanger changer) noexcept
{
	for (const auto& racketId : racketIds_)
	{
		world_->affectBody(racketId, changer);
	}
	world_->affectBody(boundaryId_, changer);
}

void Racket::addMeshInst_(BallPhysics::Id physId) noexcept
{
	BallGraphics::Graphics::MeshInstance inst;
	inst.camera = camera_;
	inst.layer = 0;
	inst.meshId = meshes_[currentLevel_];
	inst.worldFunc = [this, physId]() { return BallUtils::vectorToMatrix(world_->getPosition(physId)); };
	meshInstId_ = ui_->getGraphics().addMeshInstance(std::move(inst));
}

void Racket::removeMeshInst_() noexcept
{
	ui_->getGraphics().removeMeshInstance(meshInstId_);
}
