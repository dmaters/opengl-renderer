#include "Resources.h"

const UBOHandle UBOHandle::PROJECTION_VIEW = { 1 };
const UBOHandle UBOHandle::TEXTURES = { 2 };
const UBOHandle UBOHandle::PBR_INSTANCES = { 3 };
const UBOHandle UBOHandle::LIGHTS = { 4 };

const ProgramHandle ProgramHandle::GBUFFER = { 1 };

const MaterialHandle MaterialHandle::DEFAULT = { 1 };

const TextureHandle TextureHandle::DEFAULT_ALBEDO = { 1 };
const TextureHandle TextureHandle::DEFAULT_NORMAL = { 2 };
const TextureHandle TextureHandle::DEFAULT_ROUGHNESS_METALLIC = { 3 };
const TextureHandle TextureHandle::DEFAULT_EMISSION = { 4 };
