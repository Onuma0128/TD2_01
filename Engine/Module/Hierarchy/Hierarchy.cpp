#include "Hierarchy.h"

#include "Engine/Math/Matrix4x4.h"

void Hierarchy::initialize(const Matrix4x4& worldMatrix) noexcept {
	currentMatrix = &worldMatrix;
}

void Hierarchy::set_parent(const Hierarchy& hierarchy) noexcept {
	parent = &hierarchy;
}

void Hierarchy::reset_parent() noexcept {
	parent = nullptr;
}

const Hierarchy& Hierarchy::get_parent() const noexcept {
	return *parent;
}

bool Hierarchy::has_parent() const noexcept {
	return parent;
}

const Matrix4x4& Hierarchy::parent_matrix() const {
	return *parent->currentMatrix;
}

const Matrix4x4& Hierarchy::parent_matrix_safe() const noexcept {
	if (has_parent()) {
		return parent_matrix();
	}
	return CMatrix4x4::IDENTITY;
}
