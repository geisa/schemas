import { readdir, readFile } from "node:fs/promises";
import path from "node:path";

const ROOT = process.cwd();
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

if (hasError) {
  process.exit(1);
}

console.log(`Validated ${count} JSON file(s).`);
