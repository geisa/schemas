//-----------------------------------------------------------------------------
// Copyright 2025-2026, Contributors to the Grid Edge Interoperability &
// Security Alliance (GEISA), a Series of LF Projects, LLC
//
// Licensed under the Apache License, Version 2.0. See LICENSE.
//-----------------------------------------------------------------------------
// Validates that checked JSON files in this repo parse successfully.
// This is syntax-only validation; schema semantics live in
// validate-example-schemas.mjs so parse failures and contract failures remain
// easy to distinguish in lint output.

import { readdir, readFile } from "node:fs/promises";
import path from "node:path";

const ROOT = process.cwd();
// Skip generated, dependency, and scratch trees so lint stays focused on
// checked repository content.
const EXCLUDED_DIRS = new Set([
  ".git",
  ".codex",
  "build",
  "dist",
  "node_modules",
  "tmp"
]);

async function* walk(dir) {
  const entries = await readdir(dir, { withFileTypes: true });

  for (const entry of entries) {
    const fullPath = path.join(dir, entry.name);

    if (entry.isDirectory()) {
      // Recursive walk keeps the script repo-wide without hard-coding paths.
      if (!EXCLUDED_DIRS.has(entry.name)) {
        yield* walk(fullPath);
      }
      continue;
    }

    if (entry.isFile() && entry.name.endsWith(".json")) {
      yield fullPath;
    }
  }
}

let count = 0;
let hasError = false;

for await (const filePath of walk(ROOT)) {
  count += 1;
  const relPath = path.relative(ROOT, filePath);

  try {
    const contents = await readFile(filePath, "utf8");
    JSON.parse(contents);
  } catch (err) {
    hasError = true;
    const message = err instanceof Error ? err.message : String(err);
    console.error(`${relPath}: ${message}`);
  }
}

// Exit non-zero if any file failed so npm run lint stops at this phase.
if (hasError) {
  process.exit(1);
}

console.log(`Validated ${count} JSON file(s).`);
