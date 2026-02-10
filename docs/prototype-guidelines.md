# Prototype Guidelines & PROTOTYPE.md template

This document explains how to justify and document short-lived prototypes, mocks, and placeholders in the repository in a way that respects `ai-rules.md` while enabling pragmatic development.

## Rules for prototypes

- Short-lived mocks/stubs are permitted for prototyping when a full implementation is not feasible in the short term.
- Every PR that introduces or depends on a prototype MUST include a `PROTOTYPE.md` file in the PR root (or link to an issue) describing:
  - Why a prototype is used and why a full implementation is not feasible now.
  - Acceptance criteria/tests that validate behavior.
  - The issue number and planned timeline to replace the prototype with a production implementation.
  - Owner/maintainer who will track replacement.
- Label the PR `prototype` and get at least one maintainer sign-off.

## Template (in `.github/PROTOTYPE_TEMPLATE.md`)

Use the template provided to create `PROTOTYPE.md` in PRs. Keep it short and actionable.